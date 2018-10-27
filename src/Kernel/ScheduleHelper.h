#pragma once

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<uint32_t, bool> LambdaScheduleEvent;
        typedef Lambda<uint32_t, uint32_t, bool> LambdaSchedulePipe;
        typedef Lambda<uint32_t, uint32_t, float, bool> LambdaScheduleTiming;
        //////////////////////////////////////////////////////////////////////////
        void scheduleEvent( const ScheduleManagerInterfacePtr & _scheduler, float _delay, const LambdaScheduleEvent & _event );
        void scheduleTiming( const ScheduleManagerInterfacePtr & _scheduler, float _delay, const LambdaSchedulePipe & _pipe, const LambdaScheduleTiming & _timing, const LambdaScheduleEvent & _event );
        //////////////////////////////////////////////////////////////////////////
    }
}