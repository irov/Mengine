import Foundation
import SwiftEnvironment
import StoreKit
import UIKit

@objc(MengineStoreKitService)
@MainActor
public final class MengineStoreKitService: NSObject {
    private struct TransactionDelivery {
        let transaction: Transaction
        var failed: Bool
    }

    @objc public weak var delegate: MengineStoreKitServiceDelegate?

    private var m_generation = 0
    private var m_entitlementsRequest = 0
    private var m_listeners: [Task<Void, Never>] = []
    private var m_products: [String: Product] = [:]
    private var m_nonConsumableIdentifiers: Set<String> = []
    private var m_transactions: [UInt64: TransactionDelivery] = [:]
    private var m_finishedTransactions: Set<UInt64> = []

    deinit {
        for listener in m_listeners {
            listener.cancel()
        }
    }

    @objc public var canMakePayments: Bool {
        return AppStore.canMakePayments
    }

    @objc public func start() {
        stop()

        let generation = m_generation

        m_listeners.append(Task { [weak self] in
            for await result in Transaction.updates {
                guard let self = self, self.isCurrent(generation) else {
                    return
                }

                self.handleUpdate(result)
            }
        })

        m_listeners.append(Task { [weak self] in
            for await result in Transaction.unfinished {
                guard let self = self, self.isCurrent(generation) else {
                    return
                }

                self.handleUpdate(result)
            }
        })

        if #available(iOS 16.4, *) {
            m_listeners.append(Task { [weak self] in
                for await intent in PurchaseIntent.intents {
                    guard let self = self, self.isCurrent(generation) else {
                        return
                    }

                    self.purchase(intent.product, options: [])
                }
            })
        }
    }

    @objc public func stop() {
        m_generation += 1

        for listener in m_listeners {
            listener.cancel()
        }

        m_listeners.removeAll()
        m_products.removeAll()
        m_nonConsumableIdentifiers.removeAll()
        m_transactions.removeAll()
        m_finishedTransactions.removeAll()
    }

    @objc(requestProducts:nonConsumableIdentifiers:)
    public func requestProducts(_ consumableIdentifiers: [String], nonConsumableIdentifiers: [String]) {
        m_nonConsumableIdentifiers = Set(nonConsumableIdentifiers)
        refreshEntitlements()

        let identifiers = consumableIdentifiers + nonConsumableIdentifiers
        let generation = m_generation

        Task { [weak self] in
            let products: [Product]

            do {
                products = try await Product.products(for: identifiers)
            } catch {
                guard let self = self, self.isCurrent(generation) else {
                    return
                }

                self.delegate?.storeKitProductsFailed(String(describing: error))

                return
            }

            guard let self = self, self.isCurrent(generation) else {
                return
            }

            for product in products {
                self.m_products[product.id] = product
            }

            let received = Set(products.map { $0.id })
            let invalidIdentifiers = identifiers.filter { received.contains($0) == false }
            let descriptions = products.map { Self.describe($0) }

            self.delegate?.storeKitProductsResponse(descriptions, invalidIdentifiers: invalidIdentifiers)
        }
    }

    @objc(purchase:accountToken:)
    public func purchase(_ productIdentifier: String, accountToken: String) -> Bool {
        guard let product = m_products[productIdentifier] else {
            swiftLogError("purchase unknown product '\(productIdentifier)'")
            return false
        }

        var options = Set<Product.PurchaseOption>()

        if accountToken.isEmpty == false {
            guard let token = UUID(uuidString: accountToken) else {
                swiftLogError("purchase '\(productIdentifier)' has an invalid account token")
                return false
            }

            options.insert(.appAccountToken(token))
        }

        purchase(product, options: options)

        return true
    }

    @objc(completeTransactionDelivery:successful:)
    public func completeTransactionDelivery(_ transactionIdentifier: String, successful: Bool) {
        guard let identifier = UInt64(transactionIdentifier) else {
            swiftLogError("invalid transaction identifier '\(transactionIdentifier)'")
            return
        }

        if m_finishedTransactions.contains(identifier) == true {
            return
        }

        guard let delivery = m_transactions[identifier] else {
            return
        }

        if successful == false {
            m_transactions[identifier]?.failed = true
            return
        }

        finishTransaction(delivery.transaction)
    }

    private func finishInactiveTransaction(_ transaction: Transaction) {
        let identifier = transaction.id

        if m_finishedTransactions.contains(identifier) == true {
            return
        }

        if let delivery = m_transactions[identifier] {
            if delivery.failed == false {
                return
            }

            m_transactions.removeValue(forKey: identifier)
        }

        finishTransaction(transaction)
    }

    private func finishTransaction(_ transaction: Transaction) {
        let identifier = transaction.id

        let inserted = m_finishedTransactions.insert(identifier).inserted
        assert(inserted == true)

        let generation = m_generation

        Task { [weak self] in
            await transaction.finish()

            guard let self = self, self.isCurrent(generation) else {
                return
            }

            guard self.m_transactions.removeValue(forKey: identifier) != nil else {
                return
            }

            self.delegate?.storeKitTransactionFinished(transaction.productID, transactionIdentifier: String(identifier))
        }
    }

    @objc public func restorePurchases() {
        let generation = m_generation
        let identifiers = m_nonConsumableIdentifiers

        Task { [weak self] in
            guard self?.isCurrent(generation) == true else {
                return
            }

            do {
                try await AppStore.sync()
            } catch {
                guard let self = self, self.isCurrent(generation) else {
                    return
                }

                self.delegate?.storeKitRestoreResult([], error: String(describing: error))
                return
            }

            var transactions: [NSDictionary] = []
            var verificationFailed = false

            for await result in Transaction.currentEntitlements {
                guard self?.isCurrent(generation) == true else {
                    return
                }

                switch result {
                case .verified(let transaction):
                    if identifiers.contains(transaction.productID) == true,
                       transaction.productType != .consumable,
                       transaction.revocationDate == nil,
                       transaction.isUpgraded == false {
                        transactions.append(Self.describe(transaction))
                    }
                case .unverified(let transaction, _):
                    if identifiers.contains(transaction.productID) == true {
                        verificationFailed = true
                    }
                }
            }

            guard let self = self, self.isCurrent(generation) else {
                return
            }

            let error = verificationFailed == true ? "an entitlement could not be verified" : nil
            self.delegate?.storeKitRestoreResult(transactions, error: error)
            self.refreshEntitlements()
        }
    }

    @objc(querySubscriptionStatus:)
    public func querySubscriptionStatus(_ productIdentifier: String) {
        let generation = m_generation

        Task { [weak self] in
            let status = await Self.getSubscriptionStatus(productIdentifier)

            guard let self = self, self.isCurrent(generation) else {
                return
            }

            self.delegate?.storeKitSubscriptionStatus(productIdentifier, status: status)
        }
    }

    @objc public func showManageSubscriptions() -> Bool {
        let scenes = UIApplication.shared.connectedScenes.compactMap { $0 as? UIWindowScene }

        guard let scene = scenes.first(where: { $0.activationState == .foregroundActive }) else {
            swiftLogError("manage subscriptions without window scene")
            return false
        }

        let generation = m_generation

        Task { [weak self] in
            guard self?.isCurrent(generation) == true else {
                return
            }

            do {
                try await AppStore.showManageSubscriptions(in: scene)
            } catch {
                guard self?.isCurrent(generation) == true else {
                    return
                }

                swiftLogError("manage subscriptions failed: \(error)")
            }
        }

        return true
    }

    private func isCurrent(_ generation: Int) -> Bool {
        return m_generation == generation && Task.isCancelled == false
    }

    private func purchase(_ product: Product, options: Set<Product.PurchaseOption>) {
        let generation = m_generation
        let productIdentifier = product.id

        Task { [weak self] in
            guard self?.isCurrent(generation) == true else {
                return
            }

            let result: Product.PurchaseResult

            do {
                result = try await product.purchase(options: options)
            } catch {
                guard let self = self, self.isCurrent(generation) else {
                    return
                }

                self.delegate?.storeKitPurchaseFailed(productIdentifier, error: String(describing: error))

                return
            }

            guard let self = self, self.isCurrent(generation) else {
                return
            }

            switch result {
            case .success(let result):
                guard case .verified(let transaction) = result else {
                    self.delegate?.storeKitPurchaseFailed(productIdentifier, error: "transaction is not verified")
                    return
                }

                if let error = self.handleTransaction(transaction, fromPurchase: true) {
                    self.delegate?.storeKitPurchaseFailed(productIdentifier, error: error)
                }
            case .userCancelled:
                self.delegate?.storeKitPurchaseCancelled(productIdentifier)
            case .pending:
                self.delegate?.storeKitPurchasePending(productIdentifier)
            @unknown default:
                self.delegate?.storeKitPurchaseFailed(productIdentifier, error: "unknown purchase result")
            }
        }
    }

    private func handleUpdate(_ result: VerificationResult<Transaction>) {
        guard case .verified(let transaction) = result else {
            swiftLogError("transaction update is not verified")
            return
        }

        handleTransaction(transaction, fromPurchase: false)
    }

    @discardableResult
    private func handleTransaction(_ transaction: Transaction, fromPurchase: Bool) -> String? {
        if m_nonConsumableIdentifiers.contains(transaction.productID) == true {
            refreshEntitlements()
        }

        if transaction.revocationDate != nil || transaction.isUpgraded == true {
            finishInactiveTransaction(transaction)
            return "transaction is inactive"
        }

        if let expiration = transaction.expirationDate, expiration <= Date() {
            finishInactiveTransaction(transaction)
            return "transaction is inactive"
        }

        let identifier = transaction.id

        if m_finishedTransactions.contains(identifier) == true {
            if fromPurchase == false {
                return nil
            }

            if transaction.productType != .nonConsumable && transaction.productType != .autoRenewable {
                return "transaction is already finished"
            }
        } else {
            if let delivery = m_transactions[identifier] {
                if fromPurchase == false {
                    return nil
                }

                if delivery.failed == false {
                    return nil
                }
            }

            m_transactions[identifier] = TransactionDelivery(transaction: transaction, failed: false)
        }

        let description = Self.describe(transaction)
        delegate?.storeKitTransaction(description)

        return nil
    }

    private func refreshEntitlements() {
        let identifiers = m_nonConsumableIdentifiers

        if identifiers.isEmpty == true {
            return
        }

        let generation = m_generation

        m_entitlementsRequest += 1

        let request = m_entitlementsRequest

        Task { [weak self] in
            var entitlements = Dictionary(uniqueKeysWithValues: identifiers.map { ($0, false) })
            var unverified = Set<String>()

            for await result in Transaction.currentEntitlements {
                switch result {
                case .verified(let transaction):
                    guard entitlements[transaction.productID] != nil,
                          transaction.productType != .consumable,
                          transaction.revocationDate == nil,
                          transaction.isUpgraded == false else {
                        continue
                    }

                    entitlements[transaction.productID] = true
                case .unverified(let transaction, _):
                    unverified.insert(transaction.productID)
                }
            }

            for identifier in unverified where entitlements[identifier] == false {
                entitlements.removeValue(forKey: identifier)
            }

            guard let self = self, self.isCurrent(generation), self.m_entitlementsRequest == request else {
                return
            }

            self.delegate?.storeKitEntitlements(entitlements)
        }
    }

    private static func describe(_ product: Product) -> NSDictionary {
        var description: [String: Any] = [
            "productId": product.id,
            "type": typeName(product.type),
            "title": product.displayName,
            "description": product.description,
            "price": NSDecimalNumber(decimal: product.price).doubleValue,
            "priceFormatted": product.displayPrice,
            "currencyCode": product.priceFormatStyle.currencyCode
        ]

        if let period = product.subscription?.subscriptionPeriod {
            description["subscriptionPeriod"] = periodName(period)
        }

        return description as NSDictionary
    }

    private static func describe(_ transaction: Transaction) -> NSDictionary {
        var description: [String: Any] = [
            "transactionId": String(transaction.id),
            "originalTransactionId": String(transaction.originalID),
            "productId": transaction.productID,
            "type": typeName(transaction.productType),
            "quantity": transaction.purchasedQuantity
        ]

        if let accountToken = transaction.appAccountToken {
            description["accountToken"] = accountToken.uuidString.lowercased()
        }

        return description as NSDictionary
    }

    private static func typeName(_ type: Product.ProductType) -> String {
        switch type {
        case .consumable:
            return "consumable"
        case .nonConsumable:
            return "nonConsumable"
        case .autoRenewable:
            return "autoRenewable"
        case .nonRenewable:
            return "nonRenewable"
        default:
            return "unknown"
        }
    }

    private static func periodName(_ period: Product.SubscriptionPeriod) -> String? {
        switch period.unit {
        case .day:
            return "P\(period.value)D"
        case .week:
            return "P\(period.value)W"
        case .month:
            return "P\(period.value)M"
        case .year:
            return "P\(period.value)Y"
        @unknown default:
            return nil
        }
    }

    private static func getSubscriptionStatus(_ productIdentifier: String) async -> NSDictionary {
        do {
            let products = try await Product.products(for: [productIdentifier])

            guard let product = products.first(where: { $0.id == productIdentifier }),
                  let subscription = product.subscription else {
                return ["known": false]
            }

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
                      transaction.revocationDate == nil,
                      transaction.isUpgraded == false else {
                    continue
                }

                let entitled = status.state == .subscribed || status.state == .inGracePeriod

                guard entitled == true else {
                    continue
                }

                let expiration = status.state == .inGracePeriod
                    ? renewal.gracePeriodExpirationDate : transaction.expirationDate

                guard let expiration = expiration else {
                    unverified = true
                    continue
                }

                guard expiration > Date() else {
                    continue
                }

                let timestamp = expiration.timeIntervalSince1970

                if let previous = activeStatus?["expires_at"] as? Double, previous >= timestamp {
                    continue
                }

                activeStatus = [
                    "known": true,
                    "active": true,
                    "expires_at": timestamp,
                    "auto_renew": renewal.willAutoRenew
                ]
            }

            if let activeStatus = activeStatus {
                return activeStatus as NSDictionary
            }

            return ["known": unverified == false, "active": false]
        } catch {
            return ["known": false]
        }
    }
}
