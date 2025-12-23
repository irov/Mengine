#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/DispatchableInterface.h"

namespace Mengine
{
    class DispatcherServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "DispatcherService" )

    public:
        virtual void addDispatchable( DispatchableInterface * _dispatchable ) = 0;
        virtual void removeDispatchable( DispatchableInterface * _dispatchable ) = 0;

    public:
        virtual void preUpdate() = 0;
        virtual void update() = 0;
        virtual void postUpdate() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define DISPATCHER_SERVICE()\
    ((Mengine::DispatcherServiceInterface *)SERVICE_GET(Mengine::DispatcherServiceInterface))
//////////////////////////////////////////////////////////////////////////
