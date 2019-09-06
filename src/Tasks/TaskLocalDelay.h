#pragma once

#include "Kernel/Scheduler.h"

#include "GOAP/Task.h"

namespace Mengine
{
    class TaskLocalDelay
        : public GOAP::Task
    {
    public:
        TaskLocalDelay( float _time );
        ~TaskLocalDelay() override;

    protected:
        bool _onRun() override;
        void _onFinally() override;

    protected:
        void schedulerComplete( uint32_t _id );

    protected:
        float m_time;

        uint32_t m_id;

        class ScheduleEvent;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskLocalDelay> TaskLocalDelayPtr;
    //////////////////////////////////////////////////////////////////////////
}
