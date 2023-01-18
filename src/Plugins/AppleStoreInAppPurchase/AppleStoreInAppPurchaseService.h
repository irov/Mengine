#pragma once

#include "AppleStoreInAppPurchaseInterface.h"
#include "AppleStoreInAppPurchaseFactoryInterface.h"

#include "Interface/FactoryInterface.h"

#include "Kernel/ServiceBase.h"

#import "AppleStoreInAppPurchaseProductsRequestDelegate.h"
#import "AppleStoreInAppPurchasePaymentTransactionObserver.h"

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

namespace Mengine
{
    class AppleStoreInAppPurchaseService
        : public ServiceBase<AppleStoreInAppPurchaseServiceInterface>
        , public AppleStoreInAppPurchaseFactoryInterface
    {
    public:
        AppleStoreInAppPurchaseService();
        ~AppleStoreInAppPurchaseService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
        
    public:
        void setProvider( const AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr & _provider ) override;
        const AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr & getProvider() const override;
        
    public:
        bool canMakePayments() const override;
        AppleStoreInAppPurchaseProductsRequestInterfacePtr requestProducts( const VectorConstString & _productIdentifiers, const AppleStoreInAppPurchaseProductsResponseInterfacePtr & _cb ) override;
        
    public:
        AppleStoreInAppPurchaseProductInterfacePtr makeProduct( SKProduct * _skProduct ) override;
        AppleStoreInAppPurchaseProductsRequestInterfacePtr makeProductsRequest( SKProductsRequest * _skProductsRequest ) override;
        AppleStoreInAppPurchasePaymentTransactionInterfacePtr makePaymentTransaction( SKPaymentTransaction * _skPaymentTransaction ) override;
        
    protected:
        AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr m_provider;
        
        FactoryInterfacePtr m_factoryPaymentTransaction;
        FactoryInterfacePtr m_factoryProduct;
        FactoryInterfacePtr m_factoryProductsRequest;
        
        AppleStoreInAppPurchasePaymentTransactionObserver * m_transactionObserver;
    };
}
