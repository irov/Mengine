#pragma once

#include "Interface/SchedulerInterface.h"

#include "GOAP/TaskInterface.h"

namespace Mengine
{
    class TaskLocalDelay
        : public GOAP::TaskInterface
    {
    public:
        TaskLocalDelay( GOAP::Allocator * _allocator, float _time, const DocumentInterfacePtr & _doc );
        ~TaskLocalDelay() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        void schedulerComplete( uint32_t _id );

    protected:
        float m_time;

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr m_doc;
#endif

        uint32_t m_id;

        class ScheduleEvent;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskLocalDelay> TaskLocalDelayPtr;
    //////////////////////////////////////////////////////////////////////////
}
