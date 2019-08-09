#pragma once

#include "Kernel/Animatable.h"

#include "GOAP/Task.h"

namespace Mengine
{
    class TaskAnimatablePause
        : public GOAP::Task
    {
    public:
        explicit TaskAnimatablePause( const AnimatablePtr & _animatable );
        ~TaskAnimatablePause() override;

    protected:
        bool _onRun() override;
        void _onFinally() override;

    protected:
        AnimatablePtr m_animatable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskAnimatablePause> TaskAnimatablePausePtr;
    //////////////////////////////////////////////////////////////////////////
}