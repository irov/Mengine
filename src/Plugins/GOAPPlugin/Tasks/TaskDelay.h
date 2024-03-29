#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/DocumentInterface.h"
#include "Interface/SchedulerInterface.h"

namespace Mengine
{
    class TaskDelay
        : public GOAP::TaskInterface
    {
    public:
        TaskDelay( GOAP::Allocator * _allocator, const SchedulerInterfacePtr & _scheduler, float _time, const DocumentInterfacePtr & _doc );
        ~TaskDelay() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        void schedulerComplete( uint32_t _id );

    protected:
        SchedulerInterfacePtr m_scheduler;
        float m_time;

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr m_doc;
#endif

        uint32_t m_id;

        class ScheduleEvent;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskDelay> TaskDelayPtr;
    //////////////////////////////////////////////////////////////////////////
}
