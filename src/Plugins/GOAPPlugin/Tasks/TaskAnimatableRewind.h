#pragma once

#include "GOAP/TaskInterface.h"

#include "Kernel/Animatable.h"

namespace Mengine
{
    class TaskAnimatableRewind
        : public GOAP::TaskInterface
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