#pragma once

#include "AppleStoreInAppPurchaseInterface.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Kernel/Scriptable.h"
#endif

#import <Foundation/Foundation.h>
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
        const ConstString & getProductIdentifier() const override;
        const String & getProductTitle() const override;
        const String & getProductDescription() const override;
        const String & getProductCurrencyCode() const override;
        const String & getProductPriceFormatted() const override;
        double getProductPrice() const override;
        
    protected:
        SKProduct * m_skProduct;
        
        ConstString m_productIdentifier;
        String m_productTitle;
        String m_productDescription;
        String m_productCurrencyCode;
        String m_productPriceFormatted;
        double m_productPrice;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleStoreInAppPurchaseProduct, AppleStoreInAppPurchaseProductInterface> AppleStoreInAppPurchaseProductPtr;
    //////////////////////////////////////////////////////////////////////////
}
