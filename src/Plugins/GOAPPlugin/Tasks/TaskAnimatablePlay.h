#pragma once

#include "GOAP/TaskInterface.h"

#include "Kernel/Animatable.h"

namespace Mengine
{
    class TaskAnimatablePlay
        : public GOAP::TaskInterface
    {
    public:
        TaskAnimatablePlay( GOAP::Allocator * _allocator, const AnimatablePtr & _animatable );
        ~TaskAnimatablePlay() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        AnimatablePtr m_animatable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskAnimatablePlay> TaskAnimatablePlayPtr;
    //////////////////////////////////////////////////////////////////////////
}