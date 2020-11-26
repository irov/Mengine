#include "TaskAnimatableStop.h"

#include "Interface/AnimationInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatableStop::TaskAnimatableStop( GOAP::Allocator * _allocator, const AnimatablePtr & _animatable )
        : GOAP::TaskInterface( _allocator )
        , m_animatable( _animatable )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatableStop::~TaskAnimatableStop()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskAnimatableStop::_onRun( GOAP::NodeInterface * _node )
    {
        MENGINE_UNUSED( _node );

        AnimationInterface * animation = m_animatable->getAnimation();

        if( animation == nullptr )
        {
            return true;
        }

        animation->stop();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatableStop::_onFinally()
    {
        m_animatable = nullptr;
    }
}