#pragma once

#include "GOAP/TaskInterface.h"

#include "Kernel/Animatable.h"

namespace Mengine
{
    class TaskAnimatableResume
        : public GOAP::TaskInterface
    {
    public:
        TaskAnimatableResume( GOAP::Allocator * _allocator, const AnimatablePtr & _animatable );
        ~TaskAnimatableResume() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        AnimatablePtr m_animatable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskAnimatableResume> TaskAnimatableResumePtr;
    //////////////////////////////////////////////////////////////////////////
}