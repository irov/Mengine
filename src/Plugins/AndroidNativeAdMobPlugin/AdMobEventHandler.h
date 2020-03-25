#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"
#include "Kernel/String.h"

namespace Mengine
{
    enum EnumAdMobEventHandler
    {
        ADMOB_INITIALIZE,
        ADMOB_INTERSTITIALAD_LOAD,
        ADMOB_INTERSTITIALAD_FAILED_LOAD,
        ADMOB_INTERSTITIALAD_OPENED,
        ADMOB_INTERSTITIALAD_LEFT_APPLICATION,
        ADMOB_INTERSTITIALAD_CLOSED,
        ADMOB_REWARDEDVIDEOAD_LOAD,
        ADMOB_REWARDEDVIDEOAD_OPENED,
        ADMOB_REWARDEDVIDEOAD_STARTED,
        ADMOB_REWARDEDVIDEOAD_CLOSED,
        ADMOB_REWARDEDVIDEOAD_REWARDED,
        ADMOB_REWARDEDVIDEOAD_LEFT_APPLICATION,
        ADMOB_REWARDEDVIDEOAD_FAILED_LOAD,
        ADMOB_REWARDEDVIDEOAD_COMPLETED,
    };

    class AdMobEventHandler
        : public Mixin
    {
    public:
        virtual void onAdMobInitialized() = 0;
        
    public:
        virtual void onAdMobInterstitialAdLoaded() = 0;
        virtual void onAdMobInterstitialAdFailedToLoad( int _errorCode ) = 0;
        virtual void onAdMobInterstitialAdOpened() = 0;
        virtual void onAdMobInterstitialAdLeftApplication() = 0;
        virtual void onAdMobInterstitialAdClosed() = 0;

    public:
        virtual void onAdMobRewardedVideoAdLoaded() = 0;
        virtual void onAdMobRewardedVideoAdOpened() = 0;
        virtual void onAdMobRewardedVideoAdStarted() = 0;
        virtual void onAdMobRewardedVideoAdClosed() = 0;
        virtual void onAdMobRewardedVideoAdRewarded( const String & _rewardType, int _rewardAmount ) = 0;
        virtual void onAdMobRewardedVideoAdLeftApplication() = 0;
        virtual void onAdMobRewardedVideoAdFailedToLoad( int _errorCode ) = 0;
        virtual void onAdMobRewardedVideoAdCompleted() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AdMobEventHandler> AdMobEventHandlerPtr;
    //////////////////////////////////////////////////////////////////////////
}
