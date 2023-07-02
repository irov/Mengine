#pragma once

#include "Interface/SchedulerInterface.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void( uint32_t, bool )> LambdaScheduleEvent;
        typedef Lambda<float( uint32_t, uint32_t )> LambdaSchedulePipe;
        typedef Lambda<void( uint32_t, uint32_t, float )> LambdaScheduleTiming;
        //////////////////////////////////////////////////////////////////////////
        uint32_t schedulerEvent( const SchedulerInterfacePtr & _scheduler, float _delay, const LambdaScheduleEvent & _event, const DocumentInterfacePtr & _doc );
        uint32_t schedulerTiming( const SchedulerInterfacePtr & _scheduler, const LambdaSchedulePipe & _pipe, const LambdaScheduleTiming & _timing, const LambdaScheduleEvent & _event, const DocumentInterfacePtr & _doc );
        //////////////////////////////////////////////////////////////////////////
    }
}