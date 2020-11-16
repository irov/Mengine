#pragma once

#include "GOAP/TaskInterface.h"

#include "Kernel/Scheduler.h"

namespace Mengine
{
    class TaskGlobalDelay
        : public GOAP::TaskInterface
    {
    public:
        TaskGlobalDelay( float _time, const DocumentPtr & _doc );
        ~TaskGlobalDelay() override;

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
    typedef GOAP::IntrusivePtr<TaskGlobalDelay> TaskGlobalDelayPtr;
    //////////////////////////////////////////////////////////////////////////
}
