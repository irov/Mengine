#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/EventReceiverInterface.h"

#include "Kernel/Animatable.h"
#include "Kernel/Eventable.h"
#include "Kernel/Document.h"

namespace Mengine
{
    class TaskAnimatablePlayWait
        : public GOAP::TaskInterface
    {
    public:
        TaskAnimatablePlayWait( GOAP::Allocator * _allocator, const AnimatablePtr & _animatable, const EventablePtr & _eventable, const DocumentPtr & _doc );
        ~TaskAnimatablePlayWait() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        bool _onSkipable() const override;
        void _onSkip() override;
        void _onFinally() override;

    protected:
        AnimatablePtr m_animatable;
        EventablePtr m_eventable;

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentPtr m_doc;
#endif

        EventReceiverInterfacePtr m_receiver;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskAnimatablePlayWait> TaskAnimatablePlayWaitPtr;
    //////////////////////////////////////////////////////////////////////////
}