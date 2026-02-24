#import "AppleStoreInAppPurchaseApplicationDelegate.h"

#import "AppleStoreInAppPurchasePaymentTransactionObserver.h"
#import "AppleStoreInAppPurchaseProductsRequestDelegate.h"
#import "AppleStoreInAppPurchaseEntitlements.h"

#include "AppleStoreInAppPurchasePaymentTransaction.h"
#include "AppleStoreInAppPurchaseProduct.h"
#include "AppleStoreInAppPurchaseProductsRequest.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "AppleStoreInAppPurchaseScriptEmbedding.h"
#endif

#import "Environment/iOS/iOSLog.h"

@implementation AppleStoreInAppPurchaseApplicationDelegate
{
    Mengine::AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr m_paymentTransactionProvider;

    Mengine::FactoryInterfacePtr m_factoryPaymentTransaction;
    Mengine::FactoryInterfacePtr m_factoryProduct;
    Mengine::FactoryInterfacePtr m_factoryProductsRequest;

    BOOL m_available;
}

#pragma mark - iOSPluginApplicationDelegateInterface

static BOOL AppleStoreInAppPurchase_isAvailable() {
    if( HAS_OPTION( "applestoreinapppurchase" ) == true )
    {
        return YES;
    }

    if( HAS_OPTION( "noapplestoreinapppurchase" ) == true )
    {
        return NO;
    }

    bool available = CONFIG_VALUE_BOOLEAN( "AppleStoreInAppPurchasePlugin", "Available", true );

    return available == true ? YES : NO;
}

+ (instancetype)sharedInstance {
    static AppleStoreInAppPurchaseApplicationDelegate * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[AppleStoreInAppPurchaseApplicationDelegate alloc] init];
    });

    return sharedInstance;
}

- (instancetype)init {
    self = [super init];

    m_paymentTransactionProvider = nullptr;
    m_factoryPaymentTransaction = Mengine::Helper::makeFactoryPoolWithMutex<Mengine::AppleStoreInAppPurchasePaymentTransaction, 16>( MENGINE_DOCUMENT_FACTORABLE );
    m_factoryProduct = Mengine::Helper::makeFactoryPoolWithMutex<Mengine::AppleStoreInAppPurchaseProduct, 16>( MENGINE_DOCUMENT_FACTORABLE );
    m_factoryProductsRequest = Mengine::Helper::makeFactoryPoolWithMutex<Mengine::AppleStoreInAppPurchaseProductsRequest, 16>( MENGINE_DOCUMENT_FACTORABLE );
    m_available = AppleStoreInAppPurchase_isAvailable();

    return self;
}

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
- (void)onRunBegin {
    Mengine::Helper::addScriptEmbedding<Mengine::AppleStoreInAppPurchaseScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE );
}

- (void)onStopEnd {
    Mengine::Helper::removeScriptEmbedding<Mengine::AppleStoreInAppPurchaseScriptEmbedding>();
}
#endif

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    (void)application;
    (void)launchOptions;

    if( m_available == NO )
    {
        return YES;
    }

    // According to Apple documentation, the SKPaymentQueueDelegate must be set BEFORE adding transaction observers
    // This is critical for proper IAP functionality on iPad devices
    // The delegate implements paymentQueueShouldShowPriceConsent which is required for IAP to work correctly
    [[SKPaymentQueue defaultQueue] setDelegate:self];
    
    AppleStoreInAppPurchasePaymentTransactionObserver * paymentTransactionObserver = [AppleStoreInAppPurchasePaymentTransactionObserver sharedInstance];
    
    [[SKPaymentQueue defaultQueue] addTransactionObserver:paymentTransactionObserver];
    
    return YES;
}

- (void)applicationWillTerminate:(UIApplication *)application {
    (void)application;

    if( m_available == NO )
    {
        return;
    }

    AppleStoreInAppPurchasePaymentTransactionObserver * paymentTransactionObserver = [AppleStoreInAppPurchasePaymentTransactionObserver sharedInstance];
    
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:paymentTransactionObserver];
}

- (void)onFinalize {
    AppleStoreInAppPurchasePaymentTransactionObserver * paymentTransactionObserver = [AppleStoreInAppPurchasePaymentTransactionObserver sharedInstance];

    [[SKPaymentQueue defaultQueue] removeTransactionObserver:paymentTransactionObserver];
    [[SKPaymentQueue defaultQueue] setDelegate:nil];

    [paymentTransactionObserver deactivate];

    m_paymentTransactionProvider = nullptr;

    MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPaymentTransaction );
    MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryProduct );
    MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryProductsRequest );
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

#pragma mark - AppleStoreInAppPurchaseInterface

- (void)setPaymentTransactionProvider:(const Mengine::AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr &)paymentTransactionProvider {
    m_paymentTransactionProvider = paymentTransactionProvider;
}

- (const Mengine::AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr &)getPaymentTransactionProvider {
    return m_paymentTransactionProvider;
}

- (BOOL)canMakePayments {
    if( m_available == NO )
    {
        return NO;
    }

    if( [SKPaymentQueue canMakePayments] == NO )
    {
        return NO;
    }

    return YES;
}

- (Mengine::AppleStoreInAppPurchaseProductsRequestInterfacePtr)requestProducts:(NSSet<NSString *> *)consumableIdentifiers
                                                      nonconsumableIdentifiers:(NSSet<NSString *> *)nonconsumableIdentifiers
                                                                          cb:(const Mengine::AppleStoreInAppPurchaseProductsResponseInterfacePtr &)cb {
    if( m_available == NO )
    {
        return nullptr;
    }

    if( [consumableIdentifiers intersectsSet:nonconsumableIdentifiers] == YES )
    {
        IOS_LOGGER_ERROR( @"requestProducts: consumable: %@ nonconsumable: %@ not unique", consumableIdentifiers, nonconsumableIdentifiers );

        return nullptr;
    }

    if( [SKPaymentQueue canMakePayments] == NO )
    {
        return nullptr;
    }

    IOS_LOGGER_MESSAGE( @"requestProducts: consumable: %@ nonconsumable: %@", consumableIdentifiers, nonconsumableIdentifiers );

    [[AppleStoreInAppPurchasePaymentTransactionObserver sharedInstance] activateWithInAppPurchase:self
                                                                             consumableIdentifiers:consumableIdentifiers
                                                                          nonconsumableIdentifiers:nonconsumableIdentifiers];

    Mengine::AppleStoreInAppPurchaseProductsRequestPtr request = m_factoryProductsRequest->createObject( MENGINE_DOCUMENT_FACTORABLE );

    NSSet * productIdentifiers = [consumableIdentifiers setByAddingObjectsFromSet:nonconsumableIdentifiers];

    SKProductsRequest * skRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productIdentifiers];

    id<SKProductsRequestDelegate> delegate = [[AppleStoreInAppPurchaseProductsRequestDelegate alloc] initWithInAppPurchase:self
                                                                                                                    request:request
                                                                                                                         cb:cb];
    skRequest.delegate = delegate;

    request->setSKProductsRequest( skRequest, delegate );

    [skRequest start];

    return request;
}

- (BOOL)isOwnedProduct:(NSString *)productIdentifier {
    if( [[AppleStoreInAppPurchaseEntitlements sharedInstance] isPurchased:productIdentifier] == NO )
    {
        return NO;
    }

    return YES;
}

- (BOOL)purchaseProduct:(const Mengine::AppleStoreInAppPurchaseProductInterfacePtr &)product {
    if( m_available == NO )
    {
        return NO;
    }

    if( [SKPaymentQueue canMakePayments] == NO )
    {
        return NO;
    }

    IOS_LOGGER_MESSAGE( @"purchaseProduct: %@", product->getProductIdentifier() );

    Mengine::AppleStoreInAppPurchaseProductPtr inAppProduct = Mengine::AppleStoreInAppPurchaseProductPtr::from( product );
    SKProduct * skProduct = inAppProduct->getSKProduct();

    SKPayment * payment = [SKPayment paymentWithProduct:skProduct];
    [[SKPaymentQueue defaultQueue] addPayment:payment];

    return YES;
}

- (void)restoreCompletedTransactions {
    LOGGER_MESSAGE( "restoreCompletedTransactions" );

    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

- (Mengine::AppleStoreInAppPurchaseProductInterfacePtr)makeProduct:(SKProduct *)skProduct {
    Mengine::AppleStoreInAppPurchaseProductPtr product = m_factoryProduct->createObject( MENGINE_DOCUMENT_FACTORABLE );
    product->setSKProduct( skProduct );

    return product;
}

- (Mengine::AppleStoreInAppPurchasePaymentTransactionInterfacePtr)makePaymentTransaction:(SKPaymentTransaction *)skPaymentTransaction
                                                                                  queue:(SKPaymentQueue *)skPaymentQueue {
    Mengine::AppleStoreInAppPurchasePaymentTransactionPtr transaction = m_factoryPaymentTransaction->createObject( MENGINE_DOCUMENT_FACTORABLE );
    transaction->setSKPaymentTransaction( skPaymentTransaction );
    transaction->setSKPaymentQueue( skPaymentQueue );

    return transaction;
}

@end
