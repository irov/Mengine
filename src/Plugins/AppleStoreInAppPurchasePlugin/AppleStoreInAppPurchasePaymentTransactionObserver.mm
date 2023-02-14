#import "AppleStoreInAppPurchasePaymentTransactionObserver.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
@implementation AppleStoreInAppPurchasePaymentTransactionObserver
//////////////////////////////////////////////////////////////////////////
@synthesize m_factory;
@synthesize m_service;
//////////////////////////////////////////////////////////////////////////
#pragma mark -
//////////////////////////////////////////////////////////////////////////
- (instancetype _Nonnull)initWithFactory: (Mengine::AppleStoreInAppPurchaseFactoryInterface * _Nonnull)_factory service: (Mengine::AppleStoreInAppPurchaseServiceInterface * _Nonnull)_service {
    self = [super init];
    
    self.m_factory = _factory;
    self.m_service = _service;
    
    return self;
}
//////////////////////////////////////////////////////////////////////////
#pragma mark - SKPaymentTransactionObserver
//////////////////////////////////////////////////////////////////////////
// Sent when the transaction array has changed (additions or state changes).  Client should check state of transactions and finish as appropriate.
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray<SKPaymentTransaction *> *)transactions {
    LOGGER_MESSAGE( "SKPaymentTransactionObserver paymentQueue updatedTransactions" );
    
    const Mengine::AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr & provider = m_service->getPaymentTransactionProvider();
    
    for (SKPaymentTransaction * skPaymentTransaction in transactions)
    {
        Mengine::AppleStoreInAppPurchasePaymentTransactionInterfacePtr paymentTransaction = m_factory->makePaymentTransaction( skPaymentTransaction );
        
        SKPaymentTransactionState state = skPaymentTransaction.transactionState;
        
        switch( state )
        {
            case SKPaymentTransactionStatePurchasing:
            {
                provider->onPaymentUpdatedTransactionPurchasing( paymentTransaction );
            } break;
            case SKPaymentTransactionStatePurchased:
            {
                //[self sendMessage:HANDLE_TRANSACTION_SUCCESS withData:transaction.payment.productIdentifier];
                provider->onPaymentUpdatedTransactionPurchased( paymentTransaction );
            } break;
            case SKPaymentTransactionStateFailed:
            {
                //[self sendMessage:HANDLE_TRANSACTION_FAILED withData:transaction.payment.productIdentifier];
                provider->onPaymentUpdatedTransactionFailed( paymentTransaction );
            } break;
            case SKPaymentTransactionStateRestored:
            {
                //[self sendMessage:HANDLE_RESTORE withData:transaction.originalTransaction.payment.productIdentifier];
                provider->onPaymentUpdatedTransactionRestored( paymentTransaction );
            } break;
            case SKPaymentTransactionStateDeferred:
            {
                provider->onPaymentUpdatedTransactionDeferred( paymentTransaction );
            } break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
@end
