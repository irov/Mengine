#include "AppleStoreInAppPurchaseService.h"

#include "Environment/Apple/AppleUtils.h"
#include "Environment/Apple/AppleString.h"

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
        m_factoryPaymentTransaction = Helper::makeFactoryPool<AppleStoreInAppPurchasePaymentTransaction, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryProduct = Helper::makeFactoryPool<AppleStoreInAppPurchaseProduct, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryProductsRequest = Helper::makeFactoryPool<AppleStoreInAppPurchaseProductsRequest, 16>( MENGINE_DOCUMENT_FACTORABLE );
        
        if (@available(iOS 13.0, *)) {
            m_paymentQueueDelegate = [[AppleStoreInAppPurchasePaymentQueueDelegate alloc] initWithFactory:this service:this];
            
            [[SKPaymentQueue defaultQueue] setDelegate:m_paymentQueueDelegate];
        }
        
        [[AppleStoreInAppPurchasePaymentTransactionObserver sharedInstance] setupWithFactory:this service:this];
                
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchaseService::_finalizeService()
    {
        if (@available(iOS 13.0, *)) {
            [[SKPaymentQueue defaultQueue] setDelegate:nil];
            m_paymentQueueDelegate = nil;
        }
        
        m_paymentQueueProvider = nullptr;
        m_paymentTransactionProvider = nullptr;
        
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPaymentTransaction );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryProduct );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryProductsRequest );

        m_factoryPaymentTransaction = nullptr;
        m_factoryProduct = nullptr;
        m_factoryProductsRequest = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchaseService::setPaymentQueueProvider( const AppleStoreInAppPurchasePaymentQueueProviderInterfacePtr & _paymentQueueProvider )
    {
        m_paymentQueueProvider = _paymentQueueProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    const AppleStoreInAppPurchasePaymentQueueProviderInterfacePtr & AppleStoreInAppPurchaseService::getPaymentQueueProvider() const
    {
        return m_paymentQueueProvider;
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
    AppleStoreInAppPurchaseProductsRequestInterfacePtr AppleStoreInAppPurchaseService::requestProducts( const VectorConstString & _productIdentifiers, const AppleStoreInAppPurchaseProductsResponseInterfacePtr & _cb )
    {
        if( [SKPaymentQueue canMakePayments] == NO )
        {
            return nullptr;
        }
        
        LOGGER_MESSAGE( "requestProducts:" );
        
        NSMutableSet<NSString *> * products = [[NSMutableSet<NSString *> alloc] init];
        
        for( const ConstString & productIdentifier : _productIdentifiers )
        {
            const Char * productIdentifier_str = productIdentifier.c_str();
            
            LOGGER_MESSAGE( "productIdentitier: %s"
                , productIdentifier_str
            );
            
            [products addObject:@(productIdentifier_str)];
        }
        
        AppleStoreInAppPurchaseProductsRequestPtr request = m_factoryProductsRequest->createObject( MENGINE_DOCUMENT_FACTORABLE );
        
        SKProductsRequest * skrequest = [[SKProductsRequest alloc] initWithProductIdentifiers:products];
        
        id<SKProductsRequestDelegate> delegate = [[AppleStoreInAppPurchaseProductsRequestDelegate alloc] initWithFactory:this request:request cb:_cb];
        skrequest.delegate = delegate;
        
        request->setSKProductsRequest( skrequest, delegate );
        
        [skrequest start];
        
        return request;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleStoreInAppPurchaseService::purchaseProduct( const AppleStoreInAppPurchaseProductInterfacePtr & _product )
    {
        if( [SKPaymentQueue canMakePayments] == NO )
        {
            return false;
        }
        
        LOGGER_MESSAGE( "purchaseProduct" );
        
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
    AppleStoreInAppPurchasePaymentTransactionInterfacePtr AppleStoreInAppPurchaseService::makePaymentTransaction( SKPaymentTransaction * _skPaymentTransaction )
    {
        AppleStoreInAppPurchasePaymentTransactionPtr transaction = m_factoryPaymentTransaction->createObject( MENGINE_DOCUMENT_FACTORABLE );
        
        transaction->setSKPaymentTransaction( _skPaymentTransaction );
        
        return transaction;
    }
    //////////////////////////////////////////////////////////////////////////
}
