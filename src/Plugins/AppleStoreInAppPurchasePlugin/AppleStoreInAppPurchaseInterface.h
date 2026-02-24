#pragma once

#include "Interface/ServantInterface.h"

#import "Environment/Apple/AppleIncluder.h"
#import <StoreKit/StoreKit.h>

#include "Kernel/VectorConstString.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchaseProductInterface
        : public ServantInterface
    {
    public:
        virtual NSString * getProductIdentifier() const = 0;
        virtual NSString * getProductTitle() const = 0;
        virtual NSString * getProductDescription() const = 0;
        virtual NSString * getProductCurrencyCode() const = 0;
        virtual NSString * getProductPriceFormatted() const = 0;
        virtual double getProductPrice() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleStoreInAppPurchaseProductInterface> AppleStoreInAppPurchaseProductInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchasePaymentTransactionInterface
        : public ServantInterface
    {
    public:
        virtual NSString * getProductIdentifier() const = 0;
        
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
        : public Mixin
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
}
//////////////////////////////////////////////////////////////////////////
@protocol AppleStoreInAppPurchaseInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)setPaymentTransactionProvider:(const Mengine::AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr &)paymentTransactionProvider;
- (const Mengine::AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr &)getPaymentTransactionProvider;

- (BOOL)canMakePayments;
- (Mengine::AppleStoreInAppPurchaseProductsRequestInterfacePtr)requestProducts:(NSSet<NSString *> *)consumableIdentifiers
                                                      nonconsumableIdentifiers:(NSSet<NSString *> *)nonconsumableIdentifiers
                                                                          cb:(const Mengine::AppleStoreInAppPurchaseProductsResponseInterfacePtr &)cb;
- (BOOL)isOwnedProduct:(NSString *)productIdentifier;
- (BOOL)purchaseProduct:(const Mengine::AppleStoreInAppPurchaseProductInterfacePtr &)product;
- (void)restoreCompletedTransactions;

- (Mengine::AppleStoreInAppPurchaseProductInterfacePtr)makeProduct:(SKProduct *)skProduct;
- (Mengine::AppleStoreInAppPurchasePaymentTransactionInterfacePtr)makePaymentTransaction:(SKPaymentTransaction *)skPaymentTransaction
                                                                                  queue:(SKPaymentQueue *)skPaymentQueue;

@end
