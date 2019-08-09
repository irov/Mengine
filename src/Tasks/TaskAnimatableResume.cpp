#include "TaskAnimatableResume.h"

#include "Interface/AnimationInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatableResume::TaskAnimatableResume( const AnimatablePtr & _animatable )
        : m_animatable( _animatable )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatableResume::~TaskAnimatableResume()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskAnimatableResume::_onRun()
    {
        AnimationInterface * animation = m_animatable->getAnimation();

        if( animation == nullptr )
        {
            LOGGER_ERROR( "animatable not have animation" );

            return true;
        }

        animation->resume( 0.f );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatableResume::_onFinally()
    {
        m_animatable = nullptr;
    }
}