#pragma once

#include "GOAP/TaskInterface.h"

#include "Kernel/Animatable.h"

namespace Mengine
{
    class TaskAnimatableStop
        : public GOAP::TaskInterface
    {
    public:
        explicit TaskAnimatableStop( const AnimatablePtr & _animatable );
        ~TaskAnimatableStop() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        AnimatablePtr m_animatable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskAnimatableStop> TaskAnimatableStopPtr;
    //////////////////////////////////////////////////////////////////////////
}