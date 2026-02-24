#import "AppleStoreInAppPurchasePaymentTransactionObserver.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleString.h"
#import "Environment/iOS/iOSLog.h"

#import "AppleStoreInAppPurchaseEntitlements.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/ThreadHelper.h"

@implementation AppleStoreInAppPurchasePaymentTransactionObserver

@synthesize m_inAppPurchase;

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

- (void)activateWithInAppPurchase:(id<AppleStoreInAppPurchaseInterface> _Nonnull)inAppPurchase
            consumableIdentifiers:(NSSet<NSString *> * _Nonnull)consumableIdentifiers
         nonconsumableIdentifiers:(NSSet<NSString *> * _Nonnull)nonconsumableIdentifiers {
    @synchronized (self) {
        self.m_inAppPurchase = inAppPurchase;
        
        self.m_consumableIdentifiers = consumableIdentifiers;
        self.m_nonconsumableIdentifiers = nonconsumableIdentifiers;
        
        for (NSDictionary * value in self.m_cacheSKPaymentTransactions) {
            SKPaymentQueue * queue = value[@"queue"];
            NSArray<SKPaymentTransaction *> * transactions = value[@"transactions"];
            
            [self paymentQueue:queue updatedTransactions:transactions];
        }
    }
}

- (void)deactivate {
    @synchronized (self) {
        self.m_inAppPurchase = nil;
        
        [self.m_cacheSKPaymentTransactions removeAllObjects];
    }
}

#pragma mark - SKPaymentTransactionObserver

// Sent when the transaction array has changed (additions or state changes).  Client should check state of transactions and finish as appropriate.
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray<SKPaymentTransaction *> *)transactions {
    @synchronized (self) {
        if (self.m_inAppPurchase == nil) {
            NSDictionary * value = @{@"queue": queue, @"transactions": transactions};
            [self.m_cacheSKPaymentTransactions addObject:value];
            
            return;
        }
    }
    
    IOS_LOGGER_MESSAGE( @"SKPaymentTransactionObserver paymentQueue updatedTransactions" );
    
    Mengine::AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr copy_provider = [self.m_inAppPurchase getPaymentTransactionProvider];

    if( copy_provider == nullptr )
    {
        LOGGER_ERROR( "payment transaction provider is not set" );

        return;
    }
    
    for (SKPaymentTransaction * skPaymentTransaction in transactions)
    {
        Mengine::AppleStoreInAppPurchasePaymentTransactionInterfacePtr paymentTransaction = [self.m_inAppPurchase makePaymentTransaction:skPaymentTransaction queue:queue];
        
        SKPaymentTransactionState state = skPaymentTransaction.transactionState;
        
        switch( state )
        {
            case SKPaymentTransactionStatePurchasing:
            {
                IOS_LOGGER_MESSAGE( @"SKPaymentTransactionObserver SKPaymentTransactionStatePurchasing: %@"
                    , skPaymentTransaction.payment.productIdentifier
                );
                
                Mengine::Helper::dispatchMainThreadEvent([copy_provider, paymentTransaction]() {
                    copy_provider->onPaymentQueueUpdatedTransactionPurchasing( paymentTransaction );
                });
            } break;
            case SKPaymentTransactionStatePurchased:
            {
                if (skPaymentTransaction.originalTransaction != nil) {
                    IOS_LOGGER_MESSAGE( @"SKPaymentTransactionObserver SKPaymentTransactionStatePurchased: %@ [originalTransaction]"
                        , skPaymentTransaction.payment.productIdentifier
                    );
                    
                    if ([self.m_nonconsumableIdentifiers containsObject:skPaymentTransaction.payment.productIdentifier] == YES) {
                        [[AppleStoreInAppPurchaseEntitlements sharedInstance] add:skPaymentTransaction.payment.productIdentifier];
                    }
                    
                    Mengine::Helper::dispatchMainThreadEvent([copy_provider, paymentTransaction]() {
                        copy_provider->onPaymentQueueUpdatedTransactionRestored( paymentTransaction );
                    });
                } else {
                    IOS_LOGGER_MESSAGE( @"SKPaymentTransactionObserver SKPaymentTransactionStatePurchased: %@"
                        , skPaymentTransaction.payment.productIdentifier
                    );
                    
                    if ([self.m_nonconsumableIdentifiers containsObject:skPaymentTransaction.payment.productIdentifier] == YES) {
                        [[AppleStoreInAppPurchaseEntitlements sharedInstance] add:skPaymentTransaction.payment.productIdentifier];
                    }
                    
                    Mengine::Helper::dispatchMainThreadEvent([copy_provider, paymentTransaction]() {
                        copy_provider->onPaymentQueueUpdatedTransactionPurchased( paymentTransaction );
                    });
                }
            } break;
            case SKPaymentTransactionStateFailed:
            {
                IOS_LOGGER_MESSAGE( @"SKPaymentTransactionObserver SKPaymentTransactionStateFailed: %@"
                    , skPaymentTransaction.payment.productIdentifier
                );
                
                Mengine::Helper::dispatchMainThreadEvent([copy_provider, paymentTransaction]() {
                    copy_provider->onPaymentQueueUpdatedTransactionFailed( paymentTransaction );
                });
            } break;
            case SKPaymentTransactionStateRestored:
            {
                IOS_LOGGER_MESSAGE( @"SKPaymentTransactionObserver SKPaymentTransactionStateRestored: %@"
                    , skPaymentTransaction.payment.productIdentifier
                );
                
                if ([self.m_nonconsumableIdentifiers containsObject:skPaymentTransaction.payment.productIdentifier] == YES) {
                    [[AppleStoreInAppPurchaseEntitlements sharedInstance] add:skPaymentTransaction.payment.productIdentifier];
                }
                
                Mengine::Helper::dispatchMainThreadEvent([copy_provider, paymentTransaction]() {
                    copy_provider->onPaymentQueueUpdatedTransactionRestored( paymentTransaction );
                });
            } break;
            case SKPaymentTransactionStateDeferred:
            {
                IOS_LOGGER_MESSAGE( @"SKPaymentTransactionObserver SKPaymentTransactionStateDeferred: %@"
                    , skPaymentTransaction.payment.productIdentifier
                );
                
                Mengine::Helper::dispatchMainThreadEvent([copy_provider, paymentTransaction]() {
                    copy_provider->onPaymentQueueUpdatedTransactionDeferred( paymentTransaction );
                });
            } break;
        }
    }
}

// Sent when transactions are removed from the queue (via finishTransaction:).
- (void)paymentQueue:(SKPaymentQueue *)queue removedTransactions:(NSArray<SKPaymentTransaction *> *)transactions {
    @synchronized (self) {
        if (self.m_inAppPurchase == nil) {
            return;
        }
    }
    
    IOS_LOGGER_MESSAGE( @"SKPaymentTransactionObserver paymentQueue: %@ removedTransactions: %@"
        , queue
        , transactions
    );
    
    //ToDo
}

// Sent when an error is encountered while adding transactions from the user's purchase history back to the queue.
- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error {
    @synchronized (self) {
        if (self.m_inAppPurchase == nil) {
            return;
        }
    }
    
    IOS_LOGGER_MESSAGE( @"SKPaymentTransactionObserver paymentQueue: %@ restoreCompletedTransactionsFailedWithError: %@"
        , queue
        , [AppleDetail getMessageFromNSError:error]
    );
    
    //ToDo
}

// Sent when all transactions from the user's purchase history have successfully been added back to the queue.
- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue {
    @synchronized (self) {
        if (self.m_inAppPurchase == nil) {
            return;
        }
    }
    
    IOS_LOGGER_MESSAGE( @"SKPaymentTransactionObserver paymentQueueRestoreCompletedTransactionsFinished: %@"
        , queue
    );
    
    //ToDo
}

// Sent when a user initiates an IAP buy from the App Store
- (BOOL)paymentQueue:(SKPaymentQueue *)queue shouldAddStorePayment:(SKPayment *)payment forProduct:(SKProduct *)product {
    @synchronized (self) {
        if (self.m_inAppPurchase == nil) {
            return NO;
        }
    }
    
    IOS_LOGGER_MESSAGE( @"SKPaymentTransactionObserver paymentQueue: %@ shouldAddStorePayment: %@ forProduct: %@"
        , queue
        , payment
        , product
    );
    
    //ToDo
    
    return YES;
}

- (void)paymentQueueDidChangeStorefront:(SKPaymentQueue *)queue {
    @synchronized (self) {
        if (self.m_inAppPurchase == nil) {
            return;
        }
    }
    
    IOS_LOGGER_MESSAGE( @"SKPaymentTransactionObserver paymentQueueDidChangeStorefront: %@"
        , queue
    );
    
    //ToDo
}

// Sent when entitlements for a user have changed and access to the specified IAPs has been revoked.
- (void)paymentQueue:(SKPaymentQueue *)queue didRevokeEntitlementsForProductIdentifiers:(NSArray<NSString *> *)productIdentifiers {
    @synchronized (self) {
        if (self.m_inAppPurchase == nil) {
            return;
        }
    }
    
    IOS_LOGGER_MESSAGE( @"SKPaymentTransactionObserver paymentQueue: %@ didRevokeEntitlementsForProductIdentifiers: %@"
        , queue
        , productIdentifiers
    );
    
    for (NSString * productIdentifier in productIdentifiers) {
        if ([productIdentifier rangeOfString:@".nonconsumable."].location != NSNotFound) {
            [[AppleStoreInAppPurchaseEntitlements sharedInstance] remove:productIdentifier];
        }
    }
    
    //ToDo
}

@end
