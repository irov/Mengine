#pragma once

#include "Kernel/Animatable.h"

#include "GOAP/Task.h"

namespace Mengine
{
    class TaskAnimatableResume
        : public GOAP::Task
    {
    public:
        explicit TaskAnimatableResume( const AnimatablePtr & _animatable );
        ~TaskAnimatableResume() override;

    protected:
        bool _onRun() override;
        void _onFinally() override;

    protected:
        AnimatablePtr m_animatable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskAnimatableResume> TaskAnimatableResumePtr;
    //////////////////////////////////////////////////////////////////////////
}