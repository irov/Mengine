#pragma once

#include "AppleStoreInAppPurchaseInterface.h"
#include "AppleStoreInAppPurchaseFactoryInterface.h"

#import "Environment/Apple/AppleIncluder.h"

#include "Interface/FactoryInterface.h"

#include "Kernel/ServiceBase.h"

#import "AppleStoreInAppPurchasePaymentQueueDelegate.h"
#import "AppleStoreInAppPurchaseProductsRequestDelegate.h"
#import "AppleStoreInAppPurchasePaymentTransactionObserver.h"

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
        void setPaymentTransactionProvider( const AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr & _paymentTransactionProvider ) override;
        const AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr & getPaymentTransactionProvider() const override;
        
    public:
        bool canMakePayments() const override;
        AppleStoreInAppPurchaseProductsRequestInterfacePtr requestProducts( NSSet * _productIdentifiers, const AppleStoreInAppPurchaseProductsResponseInterfacePtr & _cb ) override;
        bool isOwnedProduct( NSString * _productIdentifier ) const override;
        bool purchaseProduct( const AppleStoreInAppPurchaseProductInterfacePtr & _product ) override;
        void restoreCompletedTransactions() override;
        
    public:
        AppleStoreInAppPurchaseProductInterfacePtr makeProduct( SKProduct * _skProduct ) override;
        AppleStoreInAppPurchasePaymentTransactionInterfacePtr makePaymentTransaction( SKPaymentTransaction * _skPaymentTransaction, SKPaymentQueue * _skPaymentQueue ) override;
        
    protected:
        AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr m_paymentTransactionProvider;
        
        FactoryInterfacePtr m_factoryPaymentTransaction;
        FactoryInterfacePtr m_factoryProduct;
        FactoryInterfacePtr m_factoryProductsRequest;
        
        AppleStoreInAppPurchasePaymentQueueDelegate * m_paymentQueueDelegate;
    };
}
