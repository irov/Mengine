#pragma once

#include "AppleStoreInAppPurchaseInterface.h"

#include "Kernel/Scriptable.h"

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchaseProductsRequest
        : public AppleStoreInAppPurchaseProductsRequestInterface
        , public Scriptable
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
