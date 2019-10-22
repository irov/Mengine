#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/UpdateContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TimepipeInterface
        : public Mixin
    {
    public:
         virtual void onTimepipe( const UpdateContext * _contet ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TimepipeInterface> TimepipeInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class TimepipeServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "TimepipeService" )

    public:
        virtual uint32_t addTimepipe( const TimepipeInterfacePtr & _timepipe ) = 0;
        virtual TimepipeInterfacePtr removeTimepipe( uint32_t _id ) = 0;

    public:
        virtual void tick( const UpdateContext * _context ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define TIMEPIPE_SERVICE()\
    ((Mengine::TimepipeServiceInterface*)SERVICE_GET(Mengine::TimepipeServiceInterface))
//////////////////////////////////////////////////////////////////////////