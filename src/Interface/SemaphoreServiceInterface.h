#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/SemaphoreListenerInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SemaphoreServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SemaphoreService" );

    public:
        virtual void activateSemaphore( const ConstString & _name ) = 0;
        virtual void deactivateSemaphore( const ConstString & _name ) = 0;

    public:        
        virtual void waitSemaphore( const ConstString & _name, const SemaphoreListenerInterfacePtr & _listener ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define SEMAPHORE_SERVICE()\
    ((Mengine::SemaphoreServiceInterface *)SERVICE_GET(Mengine::SemaphoreServiceInterface))
//////////////////////////////////////////////////////////////////////////