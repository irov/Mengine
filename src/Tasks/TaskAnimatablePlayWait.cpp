#include "TaskAnimatablePlayWait.h"

#include "Kernel/Logger.h"

#include "TaskAnimatablePlayReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatablePlayWait::TaskAnimatablePlayWait( const AnimatablePtr & _animatable, const EventablePtr& _eventable )
        : m_animatable( _animatable )
        , m_eventable( _eventable )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatablePlayWait::~TaskAnimatablePlayWait()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskAnimatablePlayWait::_onRun()
    {
        AnimationInterface * animation = m_animatable->getAnimation();

        if( animation == nullptr )
        {
            LOGGER_ERROR( "animatable not have animation" );

            return true;
        }

        EventationInterface * eventation = m_eventable->getEventation();

        if( eventation == nullptr )
        {
            LOGGER_ERROR( "eventable not have eventation" );

            return true;
        }

        TaskAnimatablePlayReceiverPtr receiver = Helper::makeFactorableUnique<TaskAnimatablePlayReceiver>();
        
        eventation->addEventReceiver( EVENT_ANIMATION_END, receiver );
        eventation->addEventReceiver( EVENT_ANIMATION_STOP, receiver );
        eventation->addEventReceiver( EVENT_ANIMATION_INTERRUPT, receiver );

        receiver->setTask( this );

        animation->play( 0.f );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskAnimatablePlayWait::_onSkipable() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlayWait::_onSkip()
    {
        AnimationInterface * animation = m_animatable->getAnimation();

        if( animation == nullptr )
        {
            LOGGER_ERROR( "animatable not have animation" );

            return;
        }

        animation->setLastFrame();
        animation->stop();
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlayWait::_onFinally()
    {
        EventationInterface * eventation = m_eventable->getEventation();

        eventation->removeEventReceiver( EVENT_ANIMATION_END );
        eventation->removeEventReceiver( EVENT_ANIMATION_STOP );
        eventation->removeEventReceiver( EVENT_ANIMATION_INTERRUPT );

        m_animatable = nullptr;
        m_eventable = nullptr;
    }
}