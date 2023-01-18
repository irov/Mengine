#pragma once

#include "AppleStoreInAppPurchaseInterface.h"

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchaseProduct
         : public AppleStoreInAppPurchaseProductInterface
    {
        DECLARE_FACTORABLE( AppleStoreInAppPurchaseProduct );
        
    public:
        AppleStoreInAppPurchaseProduct();
        ~AppleStoreInAppPurchaseProduct() override;

    public:
        void setSKProduct( SKProduct * _skProduct );
        SKProduct * getSKProduct() const;
        
    public:
        
    protected:
        SKProduct * m_skProduct;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleStoreInAppPurchaseProduct, AppleStoreInAppPurchaseProductInterface> AppleStoreInAppPurchaseProductPtr;
    //////////////////////////////////////////////////////////////////////////
}
