#include "TaskAnimatableStop.h"

#include "Interface/AnimationInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatableStop::TaskAnimatableStop( const AnimatablePtr & _animatable )
        : m_animatable( _animatable )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatableStop::~TaskAnimatableStop()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskAnimatableStop::_onRun()
    {
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