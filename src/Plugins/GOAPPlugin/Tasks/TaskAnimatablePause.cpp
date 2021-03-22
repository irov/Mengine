#include "TaskAnimatablePause.h"

#include "Interface/AnimationInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatablePause::TaskAnimatablePause( GOAP::Allocator * _allocator, const AnimatablePtr & _animatable )
        : GOAP::TaskInterface( _allocator )
        , m_animatable( _animatable )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatablePause::~TaskAnimatablePause()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskAnimatablePause::_onRun( GOAP::NodeInterface * _node )
    {
        MENGINE_UNUSED( _node );

        AnimationInterface * animation = m_animatable->getAnimation();

        if( animation == nullptr )
        {
            return true;
        }

        animation->pause();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePause::_onFinally()
    {
        m_animatable = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}