#include "TaskAnimatablePlay.h"

#include "Interface/AnimationInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatablePlay::TaskAnimatablePlay( const AnimatablePtr & _animatable )
        : m_animatable( _animatable )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatablePlay::~TaskAnimatablePlay()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskAnimatablePlay::_onRun()
    {
        AnimationInterface * animation = m_animatable->getAnimation();

        if( animation == nullptr )
        {
            return true;
        }

        animation->play( 0.f );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlay::_onFinally()
    {
        m_animatable = nullptr;
    }
}