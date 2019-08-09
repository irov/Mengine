#pragma once

#include "GOAP/Task.h"

#include "Kernel/Animatable.h"

namespace Mengine
{
    class TaskAnimatablePlay
        : public GOAP::Task
    {
    public:
        explicit TaskAnimatablePlay( const AnimatablePtr & _animatable );
        ~TaskAnimatablePlay() override;

    protected:
        bool _onRun() override;
        void _onFinally() override;

    protected:
        AnimatablePtr m_animatable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskAnimatablePlay> TaskAnimatablePlayPtr;
    //////////////////////////////////////////////////////////////////////////
}