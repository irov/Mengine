#include "AppleStoreInAppPurchaseService.h"

#import "Environment/Apple/AppleString.h"
#import "Environment/iOS/iOSLog.h"

#import "AppleStoreInAppPurchaseEntitlements.h"

#include "AppleStoreInAppPurchasePaymentTransaction.h"
#include "AppleStoreInAppPurchaseProduct.h"
#include "AppleStoreInAppPurchaseProductsRequest.h"

#include "Kernel/Logger.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/DocumentHelper.h"

////////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleStoreInAppPurchaseService, Mengine::AppleStoreInAppPurchaseService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleStoreInAppPurchaseService::AppleStoreInAppPurchaseService()
        : m_paymentQueueDelegate( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleStoreInAppPurchaseService::~AppleStoreInAppPurchaseService()
    {
    }
    /////////////////////////////////////////////////////////////////////////////
    bool AppleStoreInAppPurchaseService::_initializeService()
    {
        m_factoryPaymentTransaction = Helper::makeFactoryPoolWithMutex<AppleStoreInAppPurchasePaymentTransaction, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryProduct = Helper::makeFactoryPoolWithMutex<AppleStoreInAppPurchaseProduct, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryProductsRequest = Helper::makeFactoryPoolWithMutex<AppleStoreInAppPurchaseProductsRequest, 16>( MENGINE_DOCUMENT_FACTORABLE );
        
        if (@available(iOS 13.0, *)) {
            m_paymentQueueDelegate = [[AppleStoreInAppPurchasePaymentQueueDelegate alloc] init];
            
            [[SKPaymentQueue defaultQueue] setDelegate:m_paymentQueueDelegate];
        }
        
        [[AppleStoreInAppPurchasePaymentTransactionObserver sharedInstance] activateWithFactory:this service:this];
                
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchaseService::_finalizeService()
    {
        [[AppleStoreInAppPurchasePaymentTransactionObserver sharedInstance] deactivate];
        
        if (@available(iOS 13.0, *)) {
            [[SKPaymentQueue defaultQueue] setDelegate:nil];
            m_paymentQueueDelegate = nil;
        }
        
        m_paymentTransactionProvider = nullptr;
        
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPaymentTransaction );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryProduct );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryProductsRequest );

        m_factoryPaymentTransaction = nullptr;
        m_factoryProduct = nullptr;
        m_factoryProductsRequest = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchaseService::setPaymentTransactionProvider( const AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr & _paymentTransactionProvider )
    {
        m_paymentTransactionProvider = _paymentTransactionProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    const AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr & AppleStoreInAppPurchaseService::getPaymentTransactionProvider() const
    {
        return m_paymentTransactionProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleStoreInAppPurchaseService::canMakePayments() const
    {
        if( [SKPaymentQueue canMakePayments] == NO )
        {
            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    AppleStoreInAppPurchaseProductsRequestInterfacePtr AppleStoreInAppPurchaseService::requestProducts( NSSet * _productIdentifiers, const AppleStoreInAppPurchaseProductsResponseInterfacePtr & _cb )
    {
        if( [SKPaymentQueue canMakePayments] == NO )
        {
            return nullptr;
        }
        
        IOS_LOGGER_MESSAGE( @"requestProducts: identifiers: %@", _productIdentifiers );
        
        AppleStoreInAppPurchaseProductsRequestPtr request = m_factoryProductsRequest->createObject( MENGINE_DOCUMENT_FACTORABLE );
        
        SKProductsRequest * skrequest = [[SKProductsRequest alloc] initWithProductIdentifiers:_productIdentifiers];
        
        id<SKProductsRequestDelegate> delegate = [[AppleStoreInAppPurchaseProductsRequestDelegate alloc] initWithFactory:this request:request cb:_cb];
        skrequest.delegate = delegate;
        
        request->setSKProductsRequest( skrequest, delegate );
        
        [skrequest start];
        
        return request;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleStoreInAppPurchaseService::isOwnedProduct( NSString * _productIdentifier ) const
    {
        if( [[AppleStoreInAppPurchaseEntitlements sharedInstance] isPurchased:_productIdentifier] == NO )
        {
            return false;
        }
        
        return true;            
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleStoreInAppPurchaseService::purchaseProduct( const AppleStoreInAppPurchaseProductInterfacePtr & _product )
    {
        if( [SKPaymentQueue canMakePayments] == NO )
        {
            return false;
        }
        
        IOS_LOGGER_MESSAGE( @"purchaseProduct: %@", _product->getProductIdentifier() );
        
        AppleStoreInAppPurchaseProductPtr product = AppleStoreInAppPurchaseProductPtr::from( _product );
        
        SKProduct * skProduct = product->getSKProduct();
        
        SKPayment * payment = [SKPayment paymentWithProduct:skProduct];
        
        [[SKPaymentQueue defaultQueue] addPayment:payment];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchaseService::restoreCompletedTransactions()
    {
        LOGGER_MESSAGE( "restoreCompletedTransactions" );
        
        [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
    }
    //////////////////////////////////////////////////////////////////////////
    AppleStoreInAppPurchaseProductInterfacePtr AppleStoreInAppPurchaseService::makeProduct( SKProduct * _skProduct )
    {
        AppleStoreInAppPurchaseProductPtr product = m_factoryProduct->createObject( MENGINE_DOCUMENT_FACTORABLE );
        
        product->setSKProduct( _skProduct );
        
        return product;
    }
    //////////////////////////////////////////////////////////////////////////
    AppleStoreInAppPurchasePaymentTransactionInterfacePtr AppleStoreInAppPurchaseService::makePaymentTransaction( SKPaymentTransaction * _skPaymentTransaction, SKPaymentQueue * _skPaymentQueue )
    {
        AppleStoreInAppPurchasePaymentTransactionPtr transaction = m_factoryPaymentTransaction->createObject( MENGINE_DOCUMENT_FACTORABLE );
        
        transaction->setSKPaymentTransaction( _skPaymentTransaction );
        transaction->setSKPaymentQueue( _skPaymentQueue );
        
        return transaction;
    }
    //////////////////////////////////////////////////////////////////////////
}
