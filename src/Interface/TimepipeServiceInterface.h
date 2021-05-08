#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/TimepipeInterface.h"

#include "Kernel/Document.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TimepipeServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "TimepipeService" )

    public:
        virtual uint32_t addTimepipe( const TimepipeInterfacePtr & _timepipe, const DocumentPtr & _doc ) = 0;
        virtual TimepipeInterfacePtr removeTimepipe( uint32_t _id ) = 0;

    public:
        virtual void tick( const UpdateContext * _context ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define TIMEPIPE_SERVICE()\
    ((Mengine::TimepipeServiceInterface*)SERVICE_GET(Mengine::TimepipeServiceInterface))
//////////////////////////////////////////////////////////////////////////