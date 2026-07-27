#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/TimepipeInterface.h"
#include "Interface/DocumentInterface.h"

#include "Config/Lambda.h"
#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void( const UpdateContext * )> LambdaTimepipeCallback;
    //////////////////////////////////////////////////////////////////////////
    class TimepipeServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "TimepipeService" )

    public:
        virtual void addTimepipe( const TimepipeInterfacePtr & _timepipe, const DocumentInterfacePtr & _doc ) = 0;
        virtual void removeTimepipe( const TimepipeInterfacePtr & _timepipe ) = 0;

    public:
        virtual UniqueId addTimebeginCallback( const LambdaTimepipeCallback & _callback, const DocumentInterfacePtr & _doc ) = 0;
        virtual bool removeTimebeginCallback( UniqueId _id ) = 0;
        virtual UniqueId addTimeendCallback( const LambdaTimepipeCallback & _callback, const DocumentInterfacePtr & _doc ) = 0;
        virtual bool removeTimeendCallback( UniqueId _id ) = 0;

    public:
        virtual void tick( const UpdateContext * _context ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define TIMEPIPE_SERVICE()\
    ((Mengine::TimepipeServiceInterface *)SERVICE_GET(Mengine::TimepipeServiceInterface))
//////////////////////////////////////////////////////////////////////////
