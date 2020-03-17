#include "TaskAnimatablePause.h"

#include "Interface/AnimationInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatablePause::TaskAnimatablePause( const AnimatablePtr & _animatable )
        : m_animatable( _animatable )
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
}