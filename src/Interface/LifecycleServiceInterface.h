#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/LifecycleInterface.h"

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
        virtual void preUpdate() = 0;
        virtual void update() = 0;
        virtual void postUpdate() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define DISPATCHER_SERVICE()\
    ((Mengine::LifecycleServiceInterface *)SERVICE_GET(Mengine::LifecycleServiceInterface))
//////////////////////////////////////////////////////////////////////////
