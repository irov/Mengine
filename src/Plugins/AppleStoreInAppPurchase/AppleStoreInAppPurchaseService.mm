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
        : m_transactionObserver( nil )
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
        
        m_transactionObserver = [[AppleStoreInAppPurchasePaymentTransactionObserver alloc] initWithFactory:this service:this];
        
        [[SKPaymentQueue defaultQueue] addTransactionObserver:m_transactionObserver];
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchaseService::_finalizeService()
    {
        [[SKPaymentQueue defaultQueue] removeTransactionObserver:m_transactionObserver];
        m_transactionObserver = nil;
        
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPaymentTransaction );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryProduct );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryProductsRequest );

        m_factoryPaymentTransaction = nullptr;
        m_factoryProduct = nullptr;
        m_factoryProductsRequest = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchaseService::setProvider( const AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr & _provider )
    {
        m_provider = _provider;
    }
    //////////////////////////////////////////////////////////////////////////
    const AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr & AppleStoreInAppPurchaseService::getProvider() const
    {
        return m_provider;
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
        
        NSMutableSet<NSString *> * products = [[NSMutableSet<NSString *> alloc] init];
        
        for( const ConstString & productIdentifier : _productIdentifiers )
        {
            const Char * productIdentifier_str = productIdentifier.c_str();
            
            [products addObject:@(productIdentifier_str)];
        }
        
        SKProductsRequest * skrequest = [[SKProductsRequest alloc] initWithProductIdentifiers:products];
        skrequest.delegate = [[AppleStoreInAppPurchaseProductsRequestDelegate alloc] initWithFactory:this cb:_cb];
        
        [skrequest start];
        
        AppleStoreInAppPurchaseProductsRequestPtr request = m_factoryProductsRequest->createObject( MENGINE_DOCUMENT_FACTORABLE );
        
        request->setSKProductsRequest( skrequest );
        
        return request;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleStoreInAppPurchaseService::purchaseProduct( const AppleStoreInAppPurchaseProductInterfacePtr & _product )
    {
        if( [SKPaymentQueue canMakePayments] == NO )
        {
            return false;
        }
        
        AppleStoreInAppPurchaseProductPtr product = AppleStoreInAppPurchaseProductPtr::from( _product );
        
        SKProduct * skProduct = product->getSKProduct();
        
        SKPayment * payment = [SKPayment paymentWithProduct:skProduct];
        
        [[SKPaymentQueue defaultQueue] addPayment:payment];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchaseService::restoreCompletedTransactions()
    {
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
    AppleStoreInAppPurchaseProductsRequestInterfacePtr AppleStoreInAppPurchaseService::makeProductsRequest( SKProductsRequest * _skProductsRequest )
    {
        AppleStoreInAppPurchaseProductsRequestPtr productsRequest = m_factoryProductsRequest->createObject( MENGINE_DOCUMENT_FACTORABLE );
        
        productsRequest->setSKProductsRequest( _skProductsRequest );
        
        return productsRequest;
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
