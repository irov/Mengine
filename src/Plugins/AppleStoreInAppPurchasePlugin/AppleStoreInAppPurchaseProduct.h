#pragma once

#include "AppleStoreInAppPurchaseInterface.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Kernel/Scriptable.h"
#endif

#import <StoreKit/StoreKit.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchaseProduct
        : public AppleStoreInAppPurchaseProductInterface
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        , public Scriptable
#endif
    {
        DECLARE_FACTORABLE( AppleStoreInAppPurchaseProduct );
        
    public:
        AppleStoreInAppPurchaseProduct();
        ~AppleStoreInAppPurchaseProduct() override;

    public:
        void setSKProduct( SKProduct * _skProduct );
        SKProduct * getSKProduct() const;
        
    public:
        NSString * getProductIdentifier() const override;
        NSString * getProductTitle() const override;
        NSString * getProductDescription() const override;
        NSString * getProductCurrencyCode() const override;
        NSString * getProductPriceFormatted() const override;
        double getProductPrice() const override;
        
    protected:
        SKProduct * m_skProduct;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleStoreInAppPurchaseProduct, AppleStoreInAppPurchaseProductInterface> AppleStoreInAppPurchaseProductPtr;
    //////////////////////////////////////////////////////////////////////////
}
