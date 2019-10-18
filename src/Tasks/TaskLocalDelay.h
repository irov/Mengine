#pragma once

#include "Kernel/Scheduler.h"

#include "GOAP/Task.h"

namespace Mengine
{
    class TaskLocalDelay
        : public GOAP::Task
    {
    public:
        explicit TaskLocalDelay( float _time );
        ~TaskLocalDelay() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
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
