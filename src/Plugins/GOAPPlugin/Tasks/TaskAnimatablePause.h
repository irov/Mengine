#pragma once

#include "GOAP/TaskInterface.h"

#include "Kernel/Animatable.h"

namespace Mengine
{
    class TaskAnimatablePause
        : public GOAP::TaskInterface
    {
    public:
        explicit TaskAnimatablePause( const AnimatablePtr & _animatable );
        ~TaskAnimatablePause() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        AnimatablePtr m_animatable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskAnimatablePause> TaskAnimatablePausePtr;
    //////////////////////////////////////////////////////////////////////////
}