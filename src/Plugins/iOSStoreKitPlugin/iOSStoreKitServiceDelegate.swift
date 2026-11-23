import Foundation

@objc(MengineStoreKitServiceDelegate)
public protocol MengineStoreKitServiceDelegate: AnyObject {
    @objc(storeKitProductsResponse:invalidIdentifiers:)
    func storeKitProductsResponse(_ products: [NSDictionary], invalidIdentifiers: [String])

    @objc(storeKitProductsFailed:)
    func storeKitProductsFailed(_ error: String)

    @objc(storeKitTransaction:)
    func storeKitTransaction(_ transaction: NSDictionary)

    @objc(storeKitTransactionFinished:transactionIdentifier:)
    func storeKitTransactionFinished(_ productIdentifier: String, transactionIdentifier: String)

    @objc(storeKitRestoreResult:error:)
    func storeKitRestoreResult(_ transactions: [NSDictionary], error: String?)

    @objc(storeKitPurchaseCancelled:)
    func storeKitPurchaseCancelled(_ productIdentifier: String)

    @objc(storeKitPurchasePending:)
    func storeKitPurchasePending(_ productIdentifier: String)

    @objc(storeKitPurchaseFailed:error:)
    func storeKitPurchaseFailed(_ productIdentifier: String, error: String)

    @objc(storeKitEntitlements:)
    func storeKitEntitlements(_ entitlements: [String: Bool])

    @objc(storeKitSubscriptionStatus:status:)
    func storeKitSubscriptionStatus(_ productIdentifier: String, status: NSDictionary)
}
