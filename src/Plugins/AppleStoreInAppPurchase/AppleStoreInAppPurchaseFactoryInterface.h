#pragma once

#include "Interface/Interface.h"

#include "AppleStoreInAppPurchaseInterface.h"

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleStoreInAppPurchaseFactoryInterface
        : public Interface
    {
    public:
        virtual AppleStoreInAppPurchaseProductInterfacePtr makeProduct( SKProduct * _skProduct ) = 0;
        virtual AppleStoreInAppPurchaseProductsRequestInterfacePtr makeProductsRequest( SKProductsRequest * _transaction ) = 0;
        virtual AppleStoreInAppPurchasePaymentTransactionInterfacePtr makePaymentTransaction( SKPaymentTransaction * _transaction ) = 0;

    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleStoreInAppPurchaseFactoryInterface> AppleStoreInAppPurchaseFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
