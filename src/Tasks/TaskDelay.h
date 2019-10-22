#pragma once

#include "Kernel/Scheduler.h"

#include "GOAP/Task.h"

namespace Mengine
{
    class TaskDelay
        : public GOAP::Task
    {
    public:
        TaskDelay( const SchedulerInterfacePtr & _scheduler, float _time );
        ~TaskDelay() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        void schedulerComplete( uint32_t _id );

    protected:
        SchedulerInterfacePtr m_scheduler;
        float m_time;

        uint32_t m_id;

        class ScheduleEvent;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskDelay> TaskDelayPtr;
    //////////////////////////////////////////////////////////////////////////
}
