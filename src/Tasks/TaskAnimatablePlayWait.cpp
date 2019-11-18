#include "TaskAnimatablePlayWait.h"

#include "Interface/AnimationInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"

#include "TaskAnimatablePlayReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatablePlayWait::TaskAnimatablePlayWait( const AnimatablePtr & _animatable, const EventablePtr & _eventable )
        : m_animatable( _animatable )
        , m_eventable( _eventable )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatablePlayWait::~TaskAnimatablePlayWait()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskAnimatablePlayWait::_onRun( GOAP::NodeInterface * _node )
    {
        AnimationInterface * animation = m_animatable->getAnimation();

        if( animation == nullptr )
        {
            return true;
        }

        EventationInterface * eventation = m_eventable->getEventation();

        if( eventation == nullptr )
        {
            LOGGER_ERROR( "eventable not have eventation" );

            return true;
        }

        TaskAnimatablePlayReceiverPtr receiver = Helper::makeFactorableUnique<TaskAnimatablePlayReceiver>();

        EventReceiverInterfacePtr oldreceiver_end = eventation->addEventReceiver( EVENT_ANIMATION_END, receiver );
        EventReceiverInterfacePtr oldreceiver_stop = eventation->addEventReceiver( EVENT_ANIMATION_STOP, receiver );
        EventReceiverInterfacePtr oldreceiver_interrupt = eventation->addEventReceiver( EVENT_ANIMATION_INTERRUPT, receiver );

        MENGINE_ASSERTION_FATAL_RETURN( oldreceiver_end == nullptr, false, "event EVENT_ANIMATION_END override" );
        MENGINE_ASSERTION_FATAL_RETURN( oldreceiver_stop == nullptr, false, "event EVENT_ANIMATION_STOP override" );
        MENGINE_ASSERTION_FATAL_RETURN( oldreceiver_interrupt == nullptr, false, "event EVENT_ANIMATION_INTERRUPT override" );

        m_receiver = receiver;

        receiver->setGOAPNode( _node );

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
            return;
        }

        animation->setLastFrame();
        animation->stop();
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlayWait::_onFinally()
    {
        EventationInterface * eventation = m_eventable->getEventation();

        if( eventation != nullptr )
        {
            EventReceiverInterfacePtr delreceiver_end = eventation->removeEventReceiver( EVENT_ANIMATION_END );
            EventReceiverInterfacePtr delreceiver_stop = eventation->removeEventReceiver( EVENT_ANIMATION_STOP );
            EventReceiverInterfacePtr delreceiver_interrupt = eventation->removeEventReceiver( EVENT_ANIMATION_INTERRUPT );

            MENGINE_ASSERTION_FATAL( m_receiver == delreceiver_end, "event EVENT_ANIMATION_END miss remove" );
            MENGINE_ASSERTION_FATAL( m_receiver == delreceiver_stop, "event EVENT_ANIMATION_STOP miss remove" );
            MENGINE_ASSERTION_FATAL( m_receiver == delreceiver_interrupt, "event EVENT_ANIMATION_INTERRUPT miss remove" );
            m_receiver = nullptr;
        }

        m_animatable = nullptr;
        m_eventable = nullptr;
    }
}