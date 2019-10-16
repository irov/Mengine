#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/UpdateContext.h"

namespace Mengine
{
    class TimepipeServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "TimepipeService" )

    public:
        typedef Lambda<void( const UpdateContext * )> LambdaTimepipe;
        virtual uint32_t addTime( const LambdaTimepipe & _timepipe ) = 0;
        virtual void removeTime( uint32_t _id ) = 0;

    public:
        virtual void tick( const UpdateContext * _context ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define TIMEPIPE_SERVICE()\
    ((Mengine::TimepipeServiceInterface*)SERVICE_GET(Mengine::TimepipeServiceInterface))
//////////////////////////////////////////////////////////////////////////