#pragma once

#include "AppleStoreInAppPurchaseInterface.h"

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchaseFactoryInterface
        : public Mixin
    {
    public:
        virtual AppleStoreInAppPurchaseProductInterfacePtr makeProduct( SKProduct * _skProduct ) = 0;
        virtual AppleStoreInAppPurchasePaymentTransactionInterfacePtr makePaymentTransaction( SKPaymentTransaction * _skPaymentTransaction, SKPaymentQueue * _skPaymentQueue ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleStoreInAppPurchaseFactoryInterface> AppleStoreInAppPurchaseFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
