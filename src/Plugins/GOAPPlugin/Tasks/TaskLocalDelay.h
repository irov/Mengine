#pragma once

#include "Kernel/Scheduler.h"

#include "GOAP/TaskInterface.h"

namespace Mengine
{
    class TaskLocalDelay
        : public GOAP::TaskInterface
    {
    public:
        TaskLocalDelay( float _time, const DocumentPtr & _doc );
        ~TaskLocalDelay() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        void schedulerComplete( uint32_t _id );

    protected:
        float m_time;
        DocumentPtr m_doc;

        uint32_t m_id;

        class ScheduleEvent;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskLocalDelay> TaskLocalDelayPtr;
    //////////////////////////////////////////////////////////////////////////
}
