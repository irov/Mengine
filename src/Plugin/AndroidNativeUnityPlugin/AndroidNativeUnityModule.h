#pragma once

#include "Interface/ThreadInterface.h"

#include "Core/ModuleBase.h"

#include "UnityAdEventHandler.h"

#include "Config/Vector.h"
#include "Config/Lambda.h"

namespace Mengine 
{
    enum EUnityCommand
    {
        EUC_READY = 0,
        EUC_CLICK,
        EUC_CHANGED,
        EUC_START,
        EUC_FINISH,
        EUC_ERROR,
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void( const UnityAdEventHandlerPtr & )> LambdaUnityAdEventHandler;
    //////////////////////////////////////////////////////////////////////////
    class AndroidNativeUnityModule
        : public ModuleBase
    {
    public:
        AndroidNativeUnityModule();
        ~AndroidNativeUnityModule() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        void _update( bool _focus ) override;

    public:
        void addCommand( const LambdaUnityAdEventHandler & _command );

    public:
        const UnityAdEventHandlerPtr & getEventHandler() const;

    public:
        bool setupAds( bool _debug );
        bool showAd( const String & _placementId );
        void setAdsEventHandler( const UnityAdEventHandlerPtr & _eventHandler );
        
    protected:
        ThreadMutexInterfacePtr m_mutex;
        
        typedef Vector<LambdaUnityAdEventHandler> VectorUnityCommand;
        VectorUnityCommand m_commands;

        UnityAdEventHandlerPtr m_eventHandler;
    };
}
