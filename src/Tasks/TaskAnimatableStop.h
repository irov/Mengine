#pragma once

#include "Kernel/Animatable.h"

#include "GOAP/Task.h"

namespace Mengine
{
    class TaskAnimatableStop
        : public GOAP::Task
    {
    public:
        explicit TaskAnimatableStop( const AnimatablePtr & _animatable );
        ~TaskAnimatableStop() override;

    protected:
        bool _onRun() override;
        void _onFinally() override;

    protected:
        AnimatablePtr m_animatable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TaskAnimatableStop> TaskAnimatableStopPtr;
    //////////////////////////////////////////////////////////////////////////
}