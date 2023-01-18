#pragma once

#include "Interface/Interface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"

#include "Kernel/VectorConstString.h"

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
        virtual void finish() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleStoreInAppPurchasePaymentTransactionInterface> AppleStoreInAppPurchasePaymentTransactionInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<AppleStoreInAppPurchaseProductInterfacePtr> VectorAppleStoreInAppPurchaseProducts;
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchaseProductsResponseInterface
        : public Interface
    {
    public:
        virtual void onProductResponse( const VectorAppleStoreInAppPurchaseProducts & _products ) = 0;
        virtual void onProductFinish() = 0;
        virtual void onProductFail() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleStoreInAppPurchaseProductsResponseInterface> AppleStoreInAppPurchaseProductsResponseInterfacePtr;
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
    class AppleStoreInAppPurchasePaymentTransactionProviderInterface
        : public Interface
    {
    public:
        virtual void onPaymentUpdatedTransactionPurchasing( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) = 0;
        virtual void onPaymentUpdatedTransactionPurchased( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) = 0;
        virtual void onPaymentUpdatedTransactionFailed( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) = 0;
        virtual void onPaymentUpdatedTransactionRestored( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) = 0;
        virtual void onPaymentUpdatedTransactionDeferred( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleStoreInAppPurchasePaymentTransactionProviderInterface> AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchaseServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleStoreInAppPurchaseService" )
            
    public:
        virtual void setProvider( const AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr & _provider ) = 0;
        virtual const AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr & getProvider() const = 0;
        
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
