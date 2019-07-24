#pragma once

#include "GOAP/Task.h"

#include "Kernel/Animatable.h"
#include "Kernel/Eventable.h"

namespace Mengine
{    
    class TaskAnimatablePlayWait
        : public GOAP::Task
    {
    public:
        explicit TaskAnimatablePlayWait( const AnimatablePtr & _animatable, const EventablePtr& _eventable );
        ~TaskAnimatablePlayWait() override;

    protected:
        bool _onRun() override;
        bool _onSkipable() const override;
        void _onSkip() override;
        void _onFinally() override;

    protected:
        AnimatablePtr m_animatable;
        EventablePtr m_eventable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TaskAnimatablePlayWait> TaskAnimatablePlayWaitPtr;
    //////////////////////////////////////////////////////////////////////////
}