#import "AppleStoreInAppPurchasePaymentTransactionObserver.h"

#include "Environment/Apple/AppleErrorHelper.h"
#include "Environment/Apple/AppleString.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/PlatformHelper.h"

@implementation AppleStoreInAppPurchasePaymentTransactionObserver

@synthesize m_factory;
@synthesize m_service;

#pragma mark -

+ (instancetype _Nonnull)sharedInstance {
    static AppleStoreInAppPurchasePaymentTransactionObserver * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[AppleStoreInAppPurchasePaymentTransactionObserver alloc] init];
    });
    return sharedInstance;
}

- (instancetype _Nonnull)init {
    self = [super init];

    self.m_cacheSKPaymentTransactions = [[NSMutableArray alloc] init];
    
    return self;
}

- (void)setupWithFactory: (Mengine::AppleStoreInAppPurchaseFactoryInterface * _Nonnull)_factory service: (Mengine::AppleStoreInAppPurchaseServiceInterface * _Nonnull)_service {
    m_factory = _factory;
    m_service = _service;
}

#pragma mark - SKPaymentTransactionObserver

// Sent when the transaction array has changed (additions or state changes).  Client should check state of transactions and finish as appropriate.
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray<SKPaymentTransaction *> *)transactions {
    LOGGER_MESSAGE( "SKPaymentTransactionObserver paymentQueue updatedTransactions" );
    
    if (m_service == nil) {
        [self.m_cacheSKPaymentTransactions addObjectsFromArray:transactions];
        
        return;
    }
    
    Mengine::AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr provider = m_service->getPaymentTransactionProvider();
    
    for (SKPaymentTransaction * skPaymentTransaction in transactions)
    {
        Mengine::AppleStoreInAppPurchasePaymentTransactionInterfacePtr paymentTransaction = m_factory->makePaymentTransaction( skPaymentTransaction );
        
        SKPaymentTransactionState state = skPaymentTransaction.transactionState;
        
        switch( state )
        {
            case SKPaymentTransactionStatePurchasing:
            {
                Helper::dispatchMainThreadEvent([]() {
                    provider->onPaymentQueueUpdatedTransactionPurchasing( paymentTransaction );
                });
            } break;
            case SKPaymentTransactionStatePurchased:
            {
                Helper::dispatchMainThreadEvent([]() {
                    provider->onPaymentQueueUpdatedTransactionPurchased( paymentTransaction );
                });
            } break;
            case SKPaymentTransactionStateFailed:
            {
                Helper::dispatchMainThreadEvent([]() {
                    provider->onPaymentQueueUpdatedTransactionFailed( paymentTransaction );
                });
            } break;
            case SKPaymentTransactionStateRestored:
            {
                Helper::dispatchMainThreadEvent([]() {
                    provider->onPaymentQueueUpdatedTransactionRestored( paymentTransaction );
                });
            } break;
            case SKPaymentTransactionStateDeferred:
            {
                Helper::dispatchMainThreadEvent([]() {
                    provider->onPaymentQueueUpdatedTransactionDeferred( paymentTransaction );
                });
            } break;
        }
    }
}

// Sent when transactions are removed from the queue (via finishTransaction:).
- (void)paymentQueue:(SKPaymentQueue *)queue removedTransactions:(NSArray<SKPaymentTransaction *> *)transactions {
    LOGGER_MESSAGE( "SKPaymentTransactionObserver paymentQueue removedTransactions" );
    
    //ToDo
}

// Sent when an error is encountered while adding transactions from the user's purchase history back to the queue.
- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error {
    LOGGER_MESSAGE( "SKPaymentTransactionObserver paymentQueue restoreCompletedTransactionsFailedWithError: %s"
        , Mengine::Helper::AppleGetMessageFromNSError( error ).c_str()
    );
    
    //ToDo
}

// Sent when all transactions from the user's purchase history have successfully been added back to the queue.
- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue {
    LOGGER_MESSAGE( "SKPaymentTransactionObserver paymentQueueRestoreCompletedTransactionsFinished" );
    
    //ToDo
}

// Sent when a user initiates an IAP buy from the App Store
- (BOOL)paymentQueue:(SKPaymentQueue *)queue shouldAddStorePayment:(SKPayment *)payment forProduct:(SKProduct *)product {
    LOGGER_MESSAGE( "SKPaymentTransactionObserver shouldAddStorePayment" );
    
    //ToDo
    
    return YES;
}

- (void)paymentQueueDidChangeStorefront:(SKPaymentQueue *)queue {
    LOGGER_MESSAGE( "SKPaymentTransactionObserver paymentQueueDidChangeStorefront" );
    
    //ToDo
}

// Sent when entitlements for a user have changed and access to the specified IAPs has been revoked.
- (void)paymentQueue:(SKPaymentQueue *)queue didRevokeEntitlementsForProductIdentifiers:(NSArray<NSString *> *)productIdentifiers {
    LOGGER_MESSAGE( "SKPaymentTransactionObserver didRevokeEntitlementsForProductIdentifiers: %s"
        , Mengine::Helper::NSIdToString( productIdentifiers ).c_str()
    );
    
    //ToDo
}

@end
