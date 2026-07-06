#pragma once

#include "Interface/ServantInterface.h"

#import "Environment/Apple/AppleIncluder.h"
#import <StoreKit/StoreKit.h>

#include "Kernel/VectorConstString.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class iOSStoreInAppPurchaseProductInterface
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
    typedef IntrusivePtr<iOSStoreInAppPurchaseProductInterface> iOSStoreInAppPurchaseProductInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class iOSStoreInAppPurchasePaymentTransactionInterface
        : public ServantInterface
    {
    public:
        virtual NSString * getProductIdentifier() const = 0;

    public:
        virtual void finish() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<iOSStoreInAppPurchasePaymentTransactionInterface> iOSStoreInAppPurchasePaymentTransactionInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<iOSStoreInAppPurchaseProductInterfacePtr> VectoriOSStoreInAppPurchaseProducts;
    //////////////////////////////////////////////////////////////////////////
    class iOSStoreInAppPurchaseProductsRequestInterface
        : public ServantInterface
    {
    public:
        virtual void cancel() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<iOSStoreInAppPurchaseProductsRequestInterface> iOSStoreInAppPurchaseProductsRequestInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class iOSStoreInAppPurchaseProductsResponseInterface
        : public Mixin
    {
    public:
        virtual void onProductResponse( const iOSStoreInAppPurchaseProductsRequestInterfacePtr & _request, const VectoriOSStoreInAppPurchaseProducts & _products ) = 0;
        virtual void onProductFinish( const iOSStoreInAppPurchaseProductsRequestInterfacePtr & _request ) = 0;
        virtual void onProductFail( const iOSStoreInAppPurchaseProductsRequestInterfacePtr & _request ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<iOSStoreInAppPurchaseProductsResponseInterface> iOSStoreInAppPurchaseProductsResponseInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class iOSStoreInAppPurchasePaymentTransactionProviderInterface
        : public Mixin
    {
    public:
        virtual void onPaymentQueueUpdatedTransactionPurchasing( const iOSStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) = 0;
        virtual void onPaymentQueueUpdatedTransactionPurchased( const iOSStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) = 0;
        virtual void onPaymentQueueUpdatedTransactionFailed( const iOSStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) = 0;
        virtual void onPaymentQueueUpdatedTransactionRestored( const iOSStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) = 0;
        virtual void onPaymentQueueUpdatedTransactionDeferred( const iOSStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<iOSStoreInAppPurchasePaymentTransactionProviderInterface> iOSStoreInAppPurchasePaymentTransactionProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
@protocol iOSStoreInAppPurchaseInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)setPaymentTransactionProvider:(const Mengine::iOSStoreInAppPurchasePaymentTransactionProviderInterfacePtr &)paymentTransactionProvider;
- (const Mengine::iOSStoreInAppPurchasePaymentTransactionProviderInterfacePtr &)getPaymentTransactionProvider;

- (BOOL)canMakePayments;
- (Mengine::iOSStoreInAppPurchaseProductsRequestInterfacePtr)requestProducts:(NSSet<NSString *> *)consumableIdentifiers
                                                      nonconsumableIdentifiers:(NSSet<NSString *> *)nonconsumableIdentifiers
                                                                          cb:(const Mengine::iOSStoreInAppPurchaseProductsResponseInterfacePtr &)cb;
- (BOOL)isOwnedProduct:(NSString *)productIdentifier;
- (BOOL)purchaseProduct:(const Mengine::iOSStoreInAppPurchaseProductInterfacePtr &)product;
- (void)restoreCompletedTransactions;

- (Mengine::iOSStoreInAppPurchaseProductInterfacePtr)makeProduct:(SKProduct *)skProduct;
- (Mengine::iOSStoreInAppPurchasePaymentTransactionInterfacePtr)makePaymentTransaction:(SKPaymentTransaction *)skPaymentTransaction
                                                                                  queue:(SKPaymentQueue *)skPaymentQueue;

@end
