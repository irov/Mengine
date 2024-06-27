#pragma once

#include "Interface/SchedulerInterface.h"

#include "GOAP/TaskInterface.h"

namespace Mengine
{
    class TaskGlobalDelay
        : public GOAP::TaskInterface
    {
    public:
        TaskGlobalDelay( GOAP::Allocator * _allocator, float _time, const DocumentInterfacePtr & _doc );
        ~TaskGlobalDelay() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        void schedulerComplete( UniqueId _id );

    protected:
        float m_time;

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr m_doc;
#endif

        UniqueId m_id;

        class ScheduleEvent;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskGlobalDelay> TaskGlobalDelayPtr;
    //////////////////////////////////////////////////////////////////////////
}
