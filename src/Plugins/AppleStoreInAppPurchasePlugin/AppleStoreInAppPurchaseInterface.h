#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"

#include "Kernel/VectorConstString.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchaseProductInterface
        : public ServantInterface
    {
    public:
        virtual const ConstString & getProductIdentifier() const = 0;
        virtual const String & getProductTitle() const = 0;
        virtual const String & getProductDescription() const = 0;
        virtual const String & getProductCurrencyCode() const = 0;
        virtual const String & getProductPriceFormatted() const = 0;
        virtual double getProductPrice() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleStoreInAppPurchaseProductInterface> AppleStoreInAppPurchaseProductInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchasePaymentTransactionInterface
        : public ServantInterface
    {
    public:
        virtual const ConstString & getProductIdentifier() const = 0;
        
    public:
        virtual void finish() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleStoreInAppPurchasePaymentTransactionInterface> AppleStoreInAppPurchasePaymentTransactionInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<AppleStoreInAppPurchaseProductInterfacePtr> VectorAppleStoreInAppPurchaseProducts;
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchaseProductsRequestInterface
        : public ServantInterface
    {
    public:
        virtual void cancel() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleStoreInAppPurchaseProductsRequestInterface> AppleStoreInAppPurchaseProductsRequestInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchaseProductsResponseInterface
        : public Mixin
    {
    public:
        virtual void onProductResponse( const AppleStoreInAppPurchaseProductsRequestInterfacePtr & _request, const VectorAppleStoreInAppPurchaseProducts & _products ) = 0;
        virtual void onProductFinish( const AppleStoreInAppPurchaseProductsRequestInterfacePtr & _request ) = 0;
        virtual void onProductFail( const AppleStoreInAppPurchaseProductsRequestInterfacePtr & _request ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleStoreInAppPurchaseProductsResponseInterface> AppleStoreInAppPurchaseProductsResponseInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchasePaymentTransactionProviderInterface
        : public Interface
    {
    public:
        virtual void onPaymentQueueUpdatedTransactionPurchasing( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) = 0;
        virtual void onPaymentQueueUpdatedTransactionPurchased( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) = 0;
        virtual void onPaymentQueueUpdatedTransactionFailed( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) = 0;
        virtual void onPaymentQueueUpdatedTransactionRestored( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) = 0;
        virtual void onPaymentQueueUpdatedTransactionDeferred( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleStoreInAppPurchasePaymentTransactionProviderInterface> AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchasePaymentQueueProviderInterface
        : public Mixin
    {
    public:
        virtual void onPaymentQueueShouldContinueTransaction( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) = 0;
        virtual void onPaymentQueueShouldShowPriceConsent() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleStoreInAppPurchasePaymentQueueProviderInterface> AppleStoreInAppPurchasePaymentQueueProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchaseServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleStoreInAppPurchaseService" )
        
    public:
        virtual void setPaymentQueueProvider( const AppleStoreInAppPurchasePaymentQueueProviderInterfacePtr & _paymentQueueProvider ) = 0;
        virtual const AppleStoreInAppPurchasePaymentQueueProviderInterfacePtr & getPaymentQueueProvider() const = 0;
        
    public:
        virtual void setPaymentTransactionProvider( const AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr & _paymentTransactionProvider ) = 0;
        virtual const AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr & getPaymentTransactionProvider() const = 0;
        
    public:
        virtual bool canMakePayments() const = 0;
        virtual AppleStoreInAppPurchaseProductsRequestInterfacePtr requestProducts( const VectorConstString & _productIdentifiers, const AppleStoreInAppPurchaseProductsResponseInterfacePtr & _cb ) = 0;
        virtual bool purchaseProduct( const AppleStoreInAppPurchaseProductInterfacePtr & _product ) = 0;
        virtual void restoreCompletedTransactions() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_STOREINAPPPURCHASE_SERVICE()\
    ((Mengine::AppleStoreInAppPurchaseServiceInterface *)SERVICE_GET(Mengine::AppleStoreInAppPurchaseServiceInterface))
//////////////////////////////////////////////////////////////////////////
