#import "AppleStoreInAppPurchasePaymentTransactionObserver.h"

#import "Environment/Apple/AppleErrorHelper.h"
#import "Environment/Apple/AppleString.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/ThreadHelper.h"

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
    
    for (NSDictionary * value in self.m_cacheSKPaymentTransactions) {
        SKPaymentQueue * queue = value[@"queue"];
        NSArray<SKPaymentTransaction *> * transactions = value[@"transactions"];
        
        [self paymentQueue:queue updatedTransactions:transactions];
    }
}

#pragma mark - SKPaymentTransactionObserver

// Sent when the transaction array has changed (additions or state changes).  Client should check state of transactions and finish as appropriate.
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray<SKPaymentTransaction *> *)transactions {
    if (m_service == nil) {
        NSDictionary * value = @{@"queue": queue, @"transactions": transactions};
        [self.m_cacheSKPaymentTransactions addObject:value];
        
        return;
    }
    
    LOGGER_MESSAGE( "SKPaymentTransactionObserver paymentQueue updatedTransactions" );
    
    Mengine::AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr copy_provider = m_service->getPaymentTransactionProvider();
    
    for (SKPaymentTransaction * skPaymentTransaction in transactions)
    {
        Mengine::AppleStoreInAppPurchasePaymentTransactionInterfacePtr paymentTransaction = m_factory->makePaymentTransaction( skPaymentTransaction, queue );
        
        SKPaymentTransactionState state = skPaymentTransaction.transactionState;
        
        switch( state )
        {
            case SKPaymentTransactionStatePurchasing:
            {
                Mengine::Helper::dispatchMainThreadEvent([copy_provider, paymentTransaction]() {
                    copy_provider->onPaymentQueueUpdatedTransactionPurchasing( paymentTransaction );
                });
            } break;
            case SKPaymentTransactionStatePurchased:
            {
                Mengine::Helper::dispatchMainThreadEvent([copy_provider, paymentTransaction]() {
                    copy_provider->onPaymentQueueUpdatedTransactionPurchased( paymentTransaction );
                });
            } break;
            case SKPaymentTransactionStateFailed:
            {
                Mengine::Helper::dispatchMainThreadEvent([copy_provider, paymentTransaction]() {
                    copy_provider->onPaymentQueueUpdatedTransactionFailed( paymentTransaction );
                });
            } break;
            case SKPaymentTransactionStateRestored:
            {
                Mengine::Helper::dispatchMainThreadEvent([copy_provider, paymentTransaction]() {
                    copy_provider->onPaymentQueueUpdatedTransactionRestored( paymentTransaction );
                });
            } break;
            case SKPaymentTransactionStateDeferred:
            {
                Mengine::Helper::dispatchMainThreadEvent([copy_provider, paymentTransaction]() {
                    copy_provider->onPaymentQueueUpdatedTransactionDeferred( paymentTransaction );
                });
            } break;
        }
    }
}

// Sent when transactions are removed from the queue (via finishTransaction:).
- (void)paymentQueue:(SKPaymentQueue *)queue removedTransactions:(NSArray<SKPaymentTransaction *> *)transactions {
    if (m_service == nil) {
        return;
    }
    
    LOGGER_MESSAGE( "SKPaymentTransactionObserver paymentQueue removedTransactions" );
    
    //ToDo
}

// Sent when an error is encountered while adding transactions from the user's purchase history back to the queue.
- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error {
    if (m_service == nil) {
        return;
    }
    
    LOGGER_MESSAGE( "SKPaymentTransactionObserver paymentQueue restoreCompletedTransactionsFailedWithError: %s"
        , Mengine::Helper::AppleGetMessageFromNSError( error ).c_str()
    );
    
    //ToDo
}

// Sent when all transactions from the user's purchase history have successfully been added back to the queue.
- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue {
    if (m_service == nil) {
        return;
    }
    
    LOGGER_MESSAGE( "SKPaymentTransactionObserver paymentQueueRestoreCompletedTransactionsFinished" );
    
    //ToDo
}

// Sent when a user initiates an IAP buy from the App Store
- (BOOL)paymentQueue:(SKPaymentQueue *)queue shouldAddStorePayment:(SKPayment *)payment forProduct:(SKProduct *)product {
    if (m_service == nil) {
        return;
    }
    
    LOGGER_MESSAGE( "SKPaymentTransactionObserver shouldAddStorePayment" );
    
    //ToDo
    
    return YES;
}

- (void)paymentQueueDidChangeStorefront:(SKPaymentQueue *)queue {
    if (m_service == nil) {
        return;
    }
    
    LOGGER_MESSAGE( "SKPaymentTransactionObserver paymentQueueDidChangeStorefront" );
    
    //ToDo
}

// Sent when entitlements for a user have changed and access to the specified IAPs has been revoked.
- (void)paymentQueue:(SKPaymentQueue *)queue didRevokeEntitlementsForProductIdentifiers:(NSArray<NSString *> *)productIdentifiers {
    if (m_service == nil) {
        return;
    }
    
    LOGGER_MESSAGE( "SKPaymentTransactionObserver didRevokeEntitlementsForProductIdentifiers: %s"
        , Mengine::Helper::NSIdToString( productIdentifiers ).c_str()
    );
    
    //ToDo
}

@end
