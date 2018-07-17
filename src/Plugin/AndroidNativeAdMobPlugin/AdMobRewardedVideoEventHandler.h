#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

#include "Config/String.h"

namespace Mengine
{
    class AdMobRewardedVideoEventHandler
        : public Mixin
    {
    public:
        virtual void onRewardedVideoAdLoaded() = 0;
        virtual void onRewardedVideoAdOpened() = 0;
        virtual void onRewardedVideoStarted() = 0;
        virtual void onRewardedVideoAdClosed() = 0;
        virtual void onRewarded( const String & _rewardType, int _rewardAmount ) = 0;
        virtual void onRewardedVideoAdLeftApplication() = 0;
        virtual void onRewardedVideoAdFailedToLoad( int _errorCode ) = 0;
        virtual void onRewardedVideoCompleted() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AdMobRewardedVideoEventHandler> AdMobRewardedVideoEventHandlerPtr;
    //////////////////////////////////////////////////////////////////////////
}
