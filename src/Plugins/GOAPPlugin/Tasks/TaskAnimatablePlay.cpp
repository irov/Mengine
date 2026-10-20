#include "TaskAnimatablePlay.h"

#include "Interface/AnimationInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatablePlay::TaskAnimatablePlay( GOAP::Allocator * _allocator, const AnimatablePtr & _animatable )
        : GOAP::TaskInterface( _allocator )
        , m_animatable( _animatable )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatablePlay::~TaskAnimatablePlay()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskAnimatablePlay::_onRun( GOAP::NodeInterface * _node )
    {
        MENGINE_UNUSED( _node );

        AnimationInterface * animation = m_animatable->getAnimation();

        if( animation == nullptr )
        {
            return true;
        }

        UpdateContext context{};
        animation->play( &context );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlay::_onFinally()
    {
        m_animatable = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}