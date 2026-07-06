#import "iOSStoreInAppPurchasePlugin.h"

#import "iOSStoreInAppPurchasePaymentTransactionObserver.h"
#import "iOSStoreInAppPurchaseProductsRequestDelegate.h"
#import "iOSStoreInAppPurchaseEntitlements.h"

#include "iOSStoreInAppPurchasePaymentTransaction.h"
#include "iOSStoreInAppPurchaseProduct.h"
#include "iOSStoreInAppPurchaseProductsRequest.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "iOSStoreInAppPurchaseScriptEmbedding.h"
#   include "Kernel/ScriptEmbeddingHelper.h"
#endif

@implementation iOSStoreInAppPurchasePlugin
{
    Mengine::iOSStoreInAppPurchasePaymentTransactionProviderInterfacePtr m_paymentTransactionProvider;

    Mengine::FactoryInterfacePtr m_factoryPaymentTransaction;
    Mengine::FactoryInterfacePtr m_factoryProduct;
    Mengine::FactoryInterfacePtr m_factoryProductsRequest;
}

- (instancetype)init {
    self = [super init];

    if( self != nil )
    {
        m_paymentTransactionProvider = nullptr;
    }

    return self;
}

+ (instancetype)sharedInstance {
    static iOSStoreInAppPurchasePlugin * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[iOSStoreInAppPurchasePlugin class]];
    });

    return sharedInstance;
}

#pragma mark - iOSPluginInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    MENGINE_UNUSED( application );
    MENGINE_UNUSED( launchOptions );

    // NOTE: Mengine services (incl. ThreadSystem) are not yet initialized at this point;
    // factory pools that depend on a thread mutex are created later in onBootstrapEnd.
    return YES;
}

- (void)onBootstrapEnd {
    // Mengine bootstrap is complete here, so it is safe to allocate factory pools
    // that rely on ThreadSystemInterface (e.g. FactoryPoolWithMutex).
    m_factoryPaymentTransaction = Mengine::Helper::makeFactoryPoolWithMutex<Mengine::iOSStoreInAppPurchasePaymentTransaction, 16>( MENGINE_DOCUMENT_FACTORABLE );
    m_factoryProduct = Mengine::Helper::makeFactoryPoolWithMutex<Mengine::iOSStoreInAppPurchaseProduct, 16>( MENGINE_DOCUMENT_FACTORABLE );
    m_factoryProductsRequest = Mengine::Helper::makeFactoryPoolWithMutex<Mengine::iOSStoreInAppPurchaseProductsRequest, 16>( MENGINE_DOCUMENT_FACTORABLE );

    // According to Apple documentation, the SKPaymentQueueDelegate must be set BEFORE adding transaction observers.
    // This is critical for proper IAP functionality on iPad devices.
    // The delegate implements paymentQueueShouldShowPriceConsent which is required for IAP to work correctly.
    [[SKPaymentQueue defaultQueue] setDelegate:self];

    iOSStoreInAppPurchasePaymentTransactionObserver * paymentTransactionObserver = [iOSStoreInAppPurchasePaymentTransactionObserver sharedInstance];

    [[SKPaymentQueue defaultQueue] addTransactionObserver:paymentTransactionObserver];
}

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
- (void)onRunBegin {
    Mengine::Helper::addScriptEmbedding<Mengine::iOSStoreInAppPurchaseScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE );
}

- (void)onStopEnd {
    Mengine::Helper::removeScriptEmbedding<Mengine::iOSStoreInAppPurchaseScriptEmbedding>();
}
#endif

- (void)onFinalize {
    iOSStoreInAppPurchasePaymentTransactionObserver * paymentTransactionObserver = [iOSStoreInAppPurchasePaymentTransactionObserver sharedInstance];

    [[SKPaymentQueue defaultQueue] removeTransactionObserver:paymentTransactionObserver];
    [[SKPaymentQueue defaultQueue] setDelegate:nil];

    [paymentTransactionObserver deactivate];

    m_paymentTransactionProvider = nullptr;

    MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPaymentTransaction );
    MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryProduct );
    MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryProductsRequest );

    m_factoryPaymentTransaction = nullptr;
    m_factoryProduct = nullptr;
    m_factoryProductsRequest = nullptr;
}

#pragma mark - SKPaymentQueueDelegate

// Sent when the storefront changes while a payment is processing.
- (BOOL)paymentQueue:(SKPaymentQueue *)paymentQueue shouldContinueTransaction:(SKPaymentTransaction *)transaction inStorefront:(SKStorefront *)newStorefront {
    IOS_LOGGER_MESSAGE( @"SKPaymentQueueDelegate paymentQueue: %@ shouldContinueTransaction: %@ inStorefront: %@"
        , paymentQueue
        , transaction
        , newStorefront
    );

    return YES;
}

// Sent if there is a pending price consent confirmation from the App Store for the current user. Return YES to immediately show the price consent UI. Return NO if you intend to show it at a later time. Defaults to YES.
// This may be called at any time that you have transaction observers on the payment queue, so make sure to set the delegate before adding any transaction observers if you intend to implement this method.
- (BOOL)paymentQueueShouldShowPriceConsent:(SKPaymentQueue *)paymentQueue {
    IOS_LOGGER_MESSAGE( @"SKPaymentQueueDelegate paymentQueueShouldShowPriceConsent: %@"
        , paymentQueue
    );

    return YES;
}

#pragma mark - iOSStoreInAppPurchaseInterface

- (void)setPaymentTransactionProvider:(const Mengine::iOSStoreInAppPurchasePaymentTransactionProviderInterfacePtr &)paymentTransactionProvider {
    m_paymentTransactionProvider = paymentTransactionProvider;
}

- (const Mengine::iOSStoreInAppPurchasePaymentTransactionProviderInterfacePtr &)getPaymentTransactionProvider {
    return m_paymentTransactionProvider;
}

- (BOOL)canMakePayments {
    if( [SKPaymentQueue canMakePayments] == NO )
    {
        return NO;
    }

    return YES;
}

- (Mengine::iOSStoreInAppPurchaseProductsRequestInterfacePtr)requestProducts:(NSSet<NSString *> *)consumableIdentifiers
                                                      nonconsumableIdentifiers:(NSSet<NSString *> *)nonconsumableIdentifiers
                                                                            cb:(const Mengine::iOSStoreInAppPurchaseProductsResponseInterfacePtr &)cb {
    if( [consumableIdentifiers intersectsSet:nonconsumableIdentifiers] == YES )
    {
        IOS_LOGGER_ERROR( @"requestProducts: consumable: %@ nonconsumable: %@ not unique"
            , consumableIdentifiers
            , nonconsumableIdentifiers
        );

        return nullptr;
    }

    if( [SKPaymentQueue canMakePayments] == NO )
    {
        return nullptr;
    }

    IOS_LOGGER_MESSAGE( @"requestProducts: consumable: %@ nonconsumable: %@"
        , consumableIdentifiers
        , nonconsumableIdentifiers
    );

    [[iOSStoreInAppPurchasePaymentTransactionObserver sharedInstance] activateWithInAppPurchase:self
                                                                            consumableIdentifiers:consumableIdentifiers
                                                                         nonconsumableIdentifiers:nonconsumableIdentifiers];

    Mengine::iOSStoreInAppPurchaseProductsRequestPtr request = m_factoryProductsRequest->createObject( MENGINE_DOCUMENT_FACTORABLE );

    NSSet * productIdentifiers = [consumableIdentifiers setByAddingObjectsFromSet:nonconsumableIdentifiers];

    SKProductsRequest * skRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productIdentifiers];

    id<SKProductsRequestDelegate> delegate = [[iOSStoreInAppPurchaseProductsRequestDelegate alloc] initWithInAppPurchase:self
                                                                                                                   request:request
                                                                                                                        cb:cb];
    skRequest.delegate = delegate;

    request->setSKProductsRequest( skRequest, delegate );

    [skRequest start];

    return request;
}

- (BOOL)isOwnedProduct:(NSString *)productIdentifier {
    if( [[iOSStoreInAppPurchaseEntitlements sharedInstance] isPurchased:productIdentifier] == NO )
    {
        return NO;
    }

    return YES;
}

- (BOOL)purchaseProduct:(const Mengine::iOSStoreInAppPurchaseProductInterfacePtr &)product {
    if( [SKPaymentQueue canMakePayments] == NO )
    {
        return NO;
    }

    IOS_LOGGER_MESSAGE( @"purchaseProduct: %@"
        , product->getProductIdentifier()
    );

    Mengine::iOSStoreInAppPurchaseProductPtr inAppProduct = Mengine::iOSStoreInAppPurchaseProductPtr::from( product );
    SKProduct * skProduct = inAppProduct->getSKProduct();

    SKPayment * payment = [SKPayment paymentWithProduct:skProduct];
    [[SKPaymentQueue defaultQueue] addPayment:payment];

    return YES;
}

- (void)restoreCompletedTransactions {
    IOS_LOGGER_MESSAGE( @"restoreCompletedTransactions" );

    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

- (Mengine::iOSStoreInAppPurchaseProductInterfacePtr)makeProduct:(SKProduct *)skProduct {
    Mengine::iOSStoreInAppPurchaseProductPtr product = m_factoryProduct->createObject( MENGINE_DOCUMENT_FACTORABLE );
    product->setSKProduct( skProduct );

    return product;
}

- (Mengine::iOSStoreInAppPurchasePaymentTransactionInterfacePtr)makePaymentTransaction:(SKPaymentTransaction *)skPaymentTransaction
                                                                                   queue:(SKPaymentQueue *)skPaymentQueue {
    Mengine::iOSStoreInAppPurchasePaymentTransactionPtr transaction = m_factoryPaymentTransaction->createObject( MENGINE_DOCUMENT_FACTORABLE );
    transaction->setSKPaymentTransaction( skPaymentTransaction );
    transaction->setSKPaymentQueue( skPaymentQueue );

    return transaction;
}

@end
