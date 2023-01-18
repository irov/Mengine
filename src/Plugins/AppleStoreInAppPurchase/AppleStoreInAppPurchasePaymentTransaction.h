#pragma once

#include "AppleStoreInAppPurchaseInterface.h"

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchasePaymentTransaction
        : public AppleStoreInAppPurchasePaymentTransactionInterface
    {
        DECLARE_FACTORABLE( AppleStoreInAppPurchasePaymentTransaction );

    public:
        AppleStoreInAppPurchasePaymentTransaction();
        ~AppleStoreInAppPurchasePaymentTransaction() override;

    public:
        void setSKPaymentTransaction( SKPaymentTransaction * _skPaymentTransaction );
        SKPaymentTransaction * getSKPaymentTransaction() const;
        
    public:
        void finish() override;
        
    protected:
        SKPaymentTransaction * m_skPaymentTransaction;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleStoreInAppPurchasePaymentTransaction, AppleStoreInAppPurchasePaymentTransactionInterface> AppleStoreInAppPurchasePaymentTransactionPtr;
    //////////////////////////////////////////////////////////////////////////
}
