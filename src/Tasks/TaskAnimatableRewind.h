#pragma once

#include "Kernel/Animatable.h"

#include "GOAP/Task.h"

namespace Mengine
{
    class TaskAnimatableRewind
        : public GOAP::Task
    {
    public:
        explicit TaskAnimatableRewind( const AnimatablePtr & _animatable );
        ~TaskAnimatableRewind() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        AnimatablePtr m_animatable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskAnimatableRewind> TaskAnimatableRewindPtr;
    //////////////////////////////////////////////////////////////////////////
}