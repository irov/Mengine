#pragma once

#include "Core/PluginBase.h"

#include "AdMobInterstitialEventHandler.h"
#include "AdMobRewardedVideoEventHandler.h"

namespace Mengine 
{
    class AndroidNativeUnityPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AndroidNativeUnity" )

    public:
        AndroidNativeAdMobPlugin();
        ~AndroidNativeAdMobPlugin() override;

    protected:
        bool _avaliable() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool setupInterstitialAd();
        bool showInterstitialAd();
        bool setupRewardedVideoAd();
        bool showRewardedVideoAd();
        bool setInterstitialEventHandler( AdMobInterstitialEventHandlerPtr & _callback );
        bool setRewardedVideoEventHandler( AdMobRewardedVideoEventHandlerPtr & _callback );
    };
}
