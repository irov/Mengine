#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/EventReceiverInterface.h"

#include "Kernel/Animatable.h"
#include "Kernel/Eventable.h"

namespace Mengine
{
    class TaskAnimatablePlayWait
        : public GOAP::TaskInterface
    {
    public:
        explicit TaskAnimatablePlayWait( const AnimatablePtr & _animatable, const EventablePtr & _eventable );
        ~TaskAnimatablePlayWait() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        bool _onSkipable() const override;
        void _onSkip() override;
        void _onFinally() override;

    protected:
        AnimatablePtr m_animatable;
        EventablePtr m_eventable;

        EventReceiverInterfacePtr m_receiver;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskAnimatablePlayWait> TaskAnimatablePlayWaitPtr;
    //////////////////////////////////////////////////////////////////////////
}