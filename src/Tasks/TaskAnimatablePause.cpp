#include "TaskAnimatablePause.h"

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
    bool TaskAnimatablePause::_onRun()
    {
        AnimationInterface * animation = m_animatable->getAnimation();

        if( animation == nullptr )
        {
            LOGGER_ERROR( "animatable not have animation" );

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