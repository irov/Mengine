#pragma once

#include "Interface/ThreadInterface.h"

#include "UnityAdsEventHandler.h"

#include "Android/AndroidEventation.h"

#include "Kernel/ModuleBase.h"

#include "Config/Vector.h"
#include "Config/Lambda.h"

namespace Mengine 
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void( const UnityAdsEventHandlerPtr & )> LambdaUnityAdEventHandler;
    //////////////////////////////////////////////////////////////////////////
    class AndroidNativeUnityAdsModule
        : public ModuleBase
    {
    public:
        typedef AndroidEventation<UnityAdsEventHandler> UnityAdsEventation;
        typedef typename UnityAdsEventation::LambdaEventHandler LambdaUnityAdsEventHandler;

    public:
        AndroidNativeUnityAdsModule();
        ~AndroidNativeUnityAdsModule() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

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
