#pragma once

#include "AppleStoreInAppPurchaseInterface.h"
#include "AppleStoreInAppPurchaseFactoryInterface.h"

#include "Interface/FactoryInterface.h"

#include "Kernel/ServiceBase.h"

#import "AppleStoreInAppPurchasePaymentQueueDelegate.h"
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
        void setPaymentQueueProvider( const AppleStoreInAppPurchasePaymentQueueProviderInterfacePtr & _paymentQueueProvider ) override;
        const AppleStoreInAppPurchasePaymentQueueProviderInterfacePtr & getPaymentQueueProvider() const override;
        
    public:
        void setPaymentTransactionProvider( const AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr & _paymentTransactionProvider ) override;
        const AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr & getPaymentTransactionProvider() const override;
        
    public:
        bool canMakePayments() const override;
        AppleStoreInAppPurchaseProductsRequestInterfacePtr requestProducts( const VectorConstString & _productIdentifiers, const AppleStoreInAppPurchaseProductsResponseInterfacePtr & _cb ) override;
        bool purchaseProduct( const AppleStoreInAppPurchaseProductInterfacePtr & _product ) override;
        void restoreCompletedTransactions() override;
        
    public:
        AppleStoreInAppPurchaseProductInterfacePtr makeProduct( SKProduct * _skProduct ) override;
        AppleStoreInAppPurchaseProductsRequestInterfacePtr makeProductsRequest( SKProductsRequest * _skProductsRequest ) override;
        AppleStoreInAppPurchasePaymentTransactionInterfacePtr makePaymentTransaction( SKPaymentTransaction * _skPaymentTransaction ) override;
        
    protected:
        AppleStoreInAppPurchasePaymentQueueProviderInterfacePtr m_paymentQueueProvider;
        AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr m_paymentTransactionProvider;
        
        FactoryInterfacePtr m_factoryPaymentTransaction;
        FactoryInterfacePtr m_factoryProduct;
        FactoryInterfacePtr m_factoryProductsRequest;
        
        AppleStoreInAppPurchasePaymentQueueDelegate * m_paymentQueueDelegate;
        AppleStoreInAppPurchasePaymentTransactionObserver * m_paymentTransactionObserver;
    };
}
