#pragma once

#include "AdMobEventHandler.h"

#include "Environment/Android/AndroidEventation.h"

#include "Kernel/ModuleBase.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidNativeAdMobModule
        : public ModuleBase
    {
        DECLARE_FACTORABLE( AndroidNativeAdMobModule );

    public:
        typedef AndroidEventation<AdMobEventHandler> AdMobEventation;
        typedef typename AdMobEventation::LambdaEventHandler LambdaAdMobEventHandler;

    public:
        AndroidNativeAdMobModule();
        ~AndroidNativeAdMobModule() override;

    protected:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        void _update( bool _focus ) override;

    public:
        void addCommand( const LambdaAdMobEventHandler & _command );

    public:
        void setEventHandler( const AdMobEventHandlerPtr & _handler );

    public:
        bool initializeSDK( const String & _interAdUnitId, const String & _videoAdUnitId );

    public:
        bool setupInterstitialAd();
        bool showInterstitialAd();

    public:
        bool setupRewardedVideoAd();
        bool showRewardedVideoAd();

    protected:
        AdMobEventation m_eventation;
    };
}
