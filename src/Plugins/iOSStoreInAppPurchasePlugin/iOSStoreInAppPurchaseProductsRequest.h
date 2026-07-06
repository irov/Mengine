#pragma once

#include "iOSStoreInAppPurchaseInterface.h"

#import "Environment/Apple/AppleIncluder.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Kernel/Scriptable.h"
#endif

#import <StoreKit/StoreKit.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class iOSStoreInAppPurchaseProductsRequest
        : public iOSStoreInAppPurchaseProductsRequestInterface
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        , public Scriptable
#endif
    {
        DECLARE_FACTORABLE( iOSStoreInAppPurchaseProductsRequest );

    public:
        iOSStoreInAppPurchaseProductsRequest();
        ~iOSStoreInAppPurchaseProductsRequest() override;

    public:
        void setSKProductsRequest( SKProductsRequest * _skProductsRequest, id<SKProductsRequestDelegate> _skProductsDelegate );
        SKProductsRequest * getSKProductsRequest() const;

    public:
        void cancel() override;

    protected:
        SKProductsRequest * m_skProductsRequest;
        id<SKProductsRequestDelegate> m_skProductsDelegate;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<iOSStoreInAppPurchaseProductsRequest, iOSStoreInAppPurchaseProductsRequestInterface> iOSStoreInAppPurchaseProductsRequestPtr;
    //////////////////////////////////////////////////////////////////////////
}
