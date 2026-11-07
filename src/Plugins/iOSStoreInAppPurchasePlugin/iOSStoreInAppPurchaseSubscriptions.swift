import Foundation
import StoreKit

@objc(MengineStoreSubscriptions)
public final class MengineStoreSubscriptions: NSObject {
    private let m_lock = NSLock()
    private var m_generation = 0
    private var m_tasks: [UUID: Task<Void, Never>] = [:]

    deinit {
        cancel()
    }

    @objc public func cancel() {
        m_lock.lock()
        m_generation += 1
        let tasks = Array(m_tasks.values)
        m_tasks.removeAll()
        m_lock.unlock()
        for task in tasks { task.cancel() }
    }

    private func complete(_ identity: UUID, generation: Int) -> Bool {
        m_lock.lock()
        defer { m_lock.unlock() }
        m_tasks.removeValue(forKey: identity)
        return m_generation == generation
    }

    @objc(query:completion:)
    public func query(_ productIdentifier: String, completion: @escaping (NSDictionary) -> Void) {
        m_lock.lock()
        let generation = m_generation
        let identity = UUID()
        m_tasks[identity] = Task { @MainActor [weak self] in
            let status = await Self.getStatus(productIdentifier)
            guard let self = self else { return }
            let completed = self.complete(identity, generation: generation)
            guard completed else { return }
            guard !Task.isCancelled else { return }
            completion(status)
        }
        m_lock.unlock()
    }

    @objc(queryEntitlements:completion:)
    public func queryEntitlements(_ productIdentifiers: [String], completion: @escaping (NSDictionary) -> Void) {
        m_lock.lock()
        let generation = m_generation
        let identity = UUID()
        m_tasks[identity] = Task { @MainActor [weak self] in
            let initialEntitlements = productIdentifiers.map { ($0, false) }
            var entitlements = Dictionary(uniqueKeysWithValues: initialEntitlements)
            var unverified = Set<String>()
            for await result in Transaction.currentEntitlements {
                switch result {
                case .verified(let transaction):
                    guard entitlements[transaction.productID] != nil,
                          transaction.productType != .consumable,
                          transaction.revocationDate == nil else { continue }
                    entitlements[transaction.productID] = true
                case .unverified(let transaction, _):
                    unverified.insert(transaction.productID)
                }
            }
            for identifier in unverified where entitlements[identifier] == false {
                entitlements.removeValue(forKey: identifier)
            }
            guard let self = self else { return }
            let completed = self.complete(identity, generation: generation)
            guard completed else { return }
            guard !Task.isCancelled else { return }
            let result = entitlements as NSDictionary
            completion(result)
        }
        m_lock.unlock()
    }

    private static func getStatus(_ productIdentifier: String) async -> NSDictionary {
        do {
            let products = try await Product.products(for: [productIdentifier])
            guard let product = products.first(where: { $0.id == productIdentifier }),
                  let subscription = product.subscription else { return ["known": false] }
            let statuses = try await subscription.status
            var unverified = false
            var activeStatus: [String: Any]?
            for status in statuses {
                guard case .verified(let transaction) = status.transaction,
                      case .verified(let renewal) = status.renewalInfo else {
                    unverified = true
                    continue
                }
                guard transaction.productID == productIdentifier,
                      transaction.revocationDate == nil, !transaction.isUpgraded else { continue }
                let entitled = status.state == .subscribed || status.state == .inGracePeriod
                guard entitled else { continue }
                let expiration = status.state == .inGracePeriod
                    ? renewal.gracePeriodExpirationDate : transaction.expirationDate
                guard let expiration = expiration else {
                    unverified = true
                    continue
                }
                guard expiration > Date() else { continue }
                let timestamp = expiration.timeIntervalSince1970
                if let previous = activeStatus?["expires_at"] as? Double, previous >= timestamp { continue }
                activeStatus = ["known": true, "active": true, "expires_at": timestamp,
                                "auto_renew": renewal.willAutoRenew]
            }
            if let activeStatus = activeStatus { return activeStatus as NSDictionary }
            return ["known": !unverified, "active": false]
        } catch {
            return ["known": false]
        }
    }
}
