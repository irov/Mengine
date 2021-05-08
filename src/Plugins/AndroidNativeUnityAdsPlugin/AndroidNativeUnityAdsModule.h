#pragma once

#include "UnityAdsEventHandler.h"

#include "Environment/Android/AndroidEventation.h"

#include "Kernel/ModuleBase.h"
#include "Kernel/Vector.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void( const UnityAdsEventHandlerPtr & )> LambdaUnityAdEventHandler;
    //////////////////////////////////////////////////////////////////////////
    class AndroidNativeUnityAdsModule
        : public ModuleBase
    {
        DECLARE_FACTORABLE( AndroidNativeUnityAdsModule );

    public:
        typedef AndroidEventation<UnityAdsEventHandler> UnityAdsEventation;
        typedef typename UnityAdsEventation::LambdaEventHandler LambdaUnityAdsEventHandler;

    public:
        AndroidNativeUnityAdsModule();
        ~AndroidNativeUnityAdsModule() override;

    protected:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        void _update( bool _focus ) override;

    public:
        void addCommand( const LambdaUnityAdsEventHandler & _command );

    public:
        void setEventHandler( const UnityAdsEventHandlerPtr & _handler );

    public:
        bool initializeSDK( const String & _gameId );
        bool setupAds( bool _debug );
        bool showAd( const String & _placementId );

    protected:
        UnityAdsEventation m_eventation;
    };
}
