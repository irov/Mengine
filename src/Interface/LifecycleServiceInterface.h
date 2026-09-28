#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/LifecycleInterface.h"
#include "Interface/PluginInterface.h"

namespace Mengine
{
    class LifecycleServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "LifecycleService" )

    public:
        virtual void registerLifecycle( LifecycleInterface * _dispatchable ) = 0;
        virtual void unregisterLifecycle( LifecycleInterface * _dispatchable ) = 0;

    public:
        virtual void registerPlugin( const PluginInterfacePtr & _plugin ) = 0;
        virtual void unregisterPlugin( const PluginInterfacePtr & _plugin ) = 0;

    public:
        virtual void onApplicationDidBecomeActive() = 0;
        virtual void onApplicationWillEnterForeground() = 0;
        virtual void onApplicationDidEnterBackground() = 0;
        virtual void onApplicationWillResignActive() = 0;
        virtual void onApplicationWillTerminate() = 0;
        virtual void onApplicationDidReceiveMemoryWarning() = 0;
        virtual void onApplicationDidReceiveTrimMemory( int32_t _level ) = 0;
        virtual void onRenderDeviceCreate() = 0;
        virtual void onRenderDeviceDestroy() = 0;
        virtual void onRenderDeviceLostPrepare() = 0;
        virtual void onRenderDeviceLostRestore() = 0;

    public:
        virtual void preUpdate() = 0;
        virtual void update() = 0;
        virtual void postUpdate() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define LIFECYCLE_SERVICE()\
    ((Mengine::LifecycleServiceInterface *)SERVICE_GET(Mengine::LifecycleServiceInterface))
//////////////////////////////////////////////////////////////////////////
