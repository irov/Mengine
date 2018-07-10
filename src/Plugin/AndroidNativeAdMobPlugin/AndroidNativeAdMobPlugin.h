#pragma once

#include "Core/PluginBase.h"

#include "AdMobInterstitialEventHandler.h"
#include "AdMobRewardedVideoEventHandler.h"

namespace Mengine 
{
    class AndroidNativeAdMobPlugin
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
        bool setInterstitialEventHandler( const AdMobInterstitialEventHandlerPtr & _callback );

    public:
        bool setupRewardedVideoAd();
        bool showRewardedVideoAd();        
        bool setRewardedVideoEventHandler( const AdMobRewardedVideoEventHandlerPtr & _callback );
    };
}
