#pragma once

#include "Interface/ThreadInterface.h"

#include "Core/ModuleBase.h"

#include "Config/Lambda.h"

#include "AdMobInterstitialEventHandler.h"
#include "AdMobRewardedVideoEventHandler.h"

namespace Mengine 
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void( const AdMobInterstitialEventHandlerPtr & )> LambdaAdMobInterstitialEventHandler;
    typedef Lambda<void( const AdMobRewardedVideoEventHandlerPtr & )> LambdaAdMobRewardedVideoEventHandler;
    //////////////////////////////////////////////////////////////////////////
    class AndroidNativeAdMobModule
        : public ModuleBase
    {
    public:
        AndroidNativeAdMobModule();
        ~AndroidNativeAdMobModule() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        void _update( bool _focus ) override;

    public:
        void addInterstitialCommand( const LambdaAdMobInterstitialEventHandler & _command );
        void addRewardedVideoCommand( const LambdaAdMobRewardedVideoEventHandler & _command );

    public:
        bool setupInterstitialAd();
        bool showInterstitialAd();        
        void setInterstitialEventHandler( const AdMobInterstitialEventHandlerPtr & _callback );

    public:
        bool setupRewardedVideoAd();
        bool showRewardedVideoAd();        
        void setRewardedVideoEventHandler( const AdMobRewardedVideoEventHandlerPtr & _callback );

    protected:
        ThreadMutexInterfacePtr m_mutex;

        typedef Vector<LambdaAdMobInterstitialEventHandler> VectorAdMobInterstitialCommand;
        VectorAdMobInterstitialCommand m_interstitialCommands;

        typedef Vector<LambdaAdMobRewardedVideoEventHandler> VectorAdMobRewardedVideoCommand;
        VectorAdMobRewardedVideoCommand m_rewardedVideoCommands;

        AdMobInterstitialEventHandlerPtr m_interstitialEventHandler;
        AdMobRewardedVideoEventHandlerPtr m_rewardedVideoEventHandler;
    };
}
