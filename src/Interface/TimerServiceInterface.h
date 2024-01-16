#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/DocumentInterface.h"

#include "Config/Lambda.h"
#include "Config/UniqueId.h"

namespace Mengine
{
    class TimerServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "TimerService" );

    public:
        typedef Lambda<void( UniqueId _id )> LambdaTimer;
        virtual UniqueId addTimer( float _milliseconds, const LambdaTimer & _lambda, const DocumentInterfacePtr & _doc ) = 0;
        virtual void removeTimer( UniqueId _id ) = 0;

    public:
        virtual void update( float _time ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define TIMER_SERVICE()\
    ((Mengine::TimerServiceInterface*)SERVICE_GET(Mengine::TimerServiceInterface))
//////////////////////////////////////////////////////////////////////////