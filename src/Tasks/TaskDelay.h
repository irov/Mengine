#pragma once

#include "Kernel/Scheduler.h"

#include "GOAP/Task.h"

namespace Mengine
{
    class TaskDelay
        : public GOAP::Task
    {
    public:
        TaskDelay( const SchedulerPtr & _scheduler, float _time );
        ~TaskDelay() override;

    protected:
        bool _onRun() override;
        void _onFinally() override;

    protected:
        void schedulerComplete( uint32_t _id );

    protected:
        SchedulerPtr m_scheduler;
        float m_time;

        uint32_t m_id;

        class TaskDelayScheduleEvent;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskDelay> TaskDelayPtr;
    //////////////////////////////////////////////////////////////////////////
}
