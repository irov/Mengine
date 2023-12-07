#pragma once

#include "AppleStoreInAppPurchaseInterface.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#   include "Kernel/Scriptable.h"
#endif

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchaseProductsRequest
        : public AppleStoreInAppPurchaseProductsRequestInterface
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        , public Scriptable
#endif
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
