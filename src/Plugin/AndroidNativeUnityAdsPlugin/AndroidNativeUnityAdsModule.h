#pragma once

#include "Interface/ThreadInterface.h"

#include "Kernel/ModuleBase.h"

#include "UnityAdEventHandler.h"

#include "Config/Vector.h"
#include "Config/Lambda.h"

namespace Mengine 
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void( const UnityAdEventHandlerPtr & )> LambdaUnityAdEventHandler;
    //////////////////////////////////////////////////////////////////////////
    class AndroidNativeUnityAdsModule
        : public ModuleBase
    {
    public:
        AndroidNativeUnityAdsModule();
        ~AndroidNativeUnityAdsModule() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        void _update( bool _focus ) override;

    public:
        void addCommand( const LambdaUnityAdEventHandler & _command );
        
    public:
        bool initializeSDK( const String & _gameId );
        bool setupAds( bool _debug );
        bool showAd( const String & _placementId );
        void setAdsEventHandler( const UnityAdEventHandlerPtr & _eventHandler );
        
    protected:
        ThreadMutexInterfacePtr m_mutex;
        
        typedef Vector<LambdaUnityAdEventHandler> VectorUnityAdCommand;
        VectorUnityAdCommand m_commands;

        UnityAdEventHandlerPtr m_eventHandler;
    };
}
