#pragma once

#include "AppleStoreInAppPurchaseInterface.h"

#import "Environment/Apple/AppleIncluder.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Kernel/Scriptable.h"
#endif

#import <StoreKit/StoreKit.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchasePaymentTransaction
        : public AppleStoreInAppPurchasePaymentTransactionInterface
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        , public Scriptable
#endif
    {
        DECLARE_FACTORABLE( AppleStoreInAppPurchasePaymentTransaction );

    public:
        AppleStoreInAppPurchasePaymentTransaction();
        ~AppleStoreInAppPurchasePaymentTransaction() override;

    public:
        void setSKPaymentTransaction( SKPaymentTransaction * _skPaymentTransaction );
        SKPaymentTransaction * getSKPaymentTransaction() const;
        
        void setSKPaymentQueue( SKPaymentQueue * _skPaymentQueue );
        SKPaymentQueue * getSKPaymentQueue() const;
        
    public:
        NSString * getProductIdentifier() const override;
        
    public:
        void finish() override;
        
    protected:
        SKPaymentTransaction * m_skPaymentTransaction;
        SKPaymentQueue * m_skPaymentQueue;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleStoreInAppPurchasePaymentTransaction, AppleStoreInAppPurchasePaymentTransactionInterface> AppleStoreInAppPurchasePaymentTransactionPtr;
    //////////////////////////////////////////////////////////////////////////
}
