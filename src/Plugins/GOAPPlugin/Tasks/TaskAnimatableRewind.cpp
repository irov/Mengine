#include "TaskAnimatableRewind.h"

#include "Interface/AnimationInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatableRewind::TaskAnimatableRewind( GOAP::Allocator * _allocator, const AnimatablePtr & _animatable )
        : GOAP::TaskInterface( _allocator )
        , m_animatable( _animatable )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatableRewind::~TaskAnimatableRewind()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskAnimatableRewind::_onRun( GOAP::NodeInterface * _node )
    {
        MENGINE_UNUSED( _node );

        AnimationInterface * animation = m_animatable->getAnimation();

        if( animation == nullptr )
        {
            return true;
        }

        animation->setFirstFrame();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatableRewind::_onFinally()
    {
        m_animatable = nullptr;
    }
}