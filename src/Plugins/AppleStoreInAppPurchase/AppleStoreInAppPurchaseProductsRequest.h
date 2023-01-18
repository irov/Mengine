#pragma once

#include "AppleStoreInAppPurchaseInterface.h"

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchaseProductsRequest
         : public AppleStoreInAppPurchaseProductsRequestInterface
    {
        DECLARE_FACTORABLE( AppleStoreInAppPurchaseProductsRequest );

    public:
        AppleStoreInAppPurchaseProductsRequest();
        ~AppleStoreInAppPurchaseProductsRequest() override;

    public:
        void setSKProductsRequest( SKProductsRequest * _skProductsRequest );
        SKProductsRequest * getSKProductsRequest() const;
        
    public:
        void cancel() override;
        
    protected:
        SKProductsRequest * m_skProductsRequest;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleStoreInAppPurchaseProductsRequest, AppleStoreInAppPurchaseProductsRequestInterface> AppleStoreInAppPurchaseProductsRequestPtr;
    //////////////////////////////////////////////////////////////////////////
}
