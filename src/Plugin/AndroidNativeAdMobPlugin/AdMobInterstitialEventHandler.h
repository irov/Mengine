#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

#include "Config/String.h"

namespace Mengine
{
    class AdMobInterstitialEventHandler
        : public Mixin
    {
    public:
        virtual void onAdLoaded() = 0;
        virtual void onAdFailedToLoad( int _errorCode ) = 0;
        virtual void onAdOpened() = 0;
        virtual void onAdLeftApplication() = 0;
        virtual void onAdClosed() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AdMobInterstitialEventHandler> AdMobInterstitialEventHandlerPtr;
    //////////////////////////////////////////////////////////////////////////
}
