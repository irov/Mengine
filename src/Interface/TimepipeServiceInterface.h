#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/TimepipeInterface.h"
#include "Interface/DocumentInterface.h"

namespace Mengine
{
    class TimepipeServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "TimepipeService" )

    public:
        virtual void addTimepipe( const TimepipeInterfacePtr & _timepipe, const DocumentInterfacePtr & _doc ) = 0;
        virtual void removeTimepipe( const TimepipeInterfacePtr & _timepipe ) = 0;

    public:
        virtual void tick( const UpdateContext * _context ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define TIMEPIPE_SERVICE()\
    ((Mengine::TimepipeServiceInterface *)SERVICE_GET(Mengine::TimepipeServiceInterface))
//////////////////////////////////////////////////////////////////////////