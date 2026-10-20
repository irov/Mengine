#include "TaskAnimatableResume.h"

#include "Interface/AnimationInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatableResume::TaskAnimatableResume( GOAP::Allocator * _allocator, const AnimatablePtr & _animatable )
        : GOAP::TaskInterface( _allocator )
        , m_animatable( _animatable )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatableResume::~TaskAnimatableResume()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskAnimatableResume::_onRun( GOAP::NodeInterface * _node )
    {
        MENGINE_UNUSED( _node );

        AnimationInterface * animation = m_animatable->getAnimation();

        if( animation == nullptr )
        {
            return true;
        }

        UpdateContext context{};
        animation->resume( &context );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatableResume::_onFinally()
    {
        m_animatable = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}