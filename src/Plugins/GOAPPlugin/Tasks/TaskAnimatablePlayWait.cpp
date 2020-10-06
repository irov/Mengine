#include "TaskAnimatablePlayWait.h"

#include "Interface/AnimationInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/DocumentHelper.h"

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

        MENGINE_ASSERTION_MEMORY_PANIC( animation, "node not have animation" );

        EventationInterface * eventation = m_eventable->getEventation();

        MENGINE_ASSERTION_MEMORY_PANIC( eventation, "node not have eventation" );

        TaskAnimatablePlayReceiverPtr receiver = Helper::makeFactorableUnique<TaskAnimatablePlayReceiver>( MENGINE_DOCUMENT_FUNCTION );

        EventReceiverInterfacePtr oldreceiver_end = eventation->addEventReceiver( EVENT_ANIMATION_END, receiver );
        EventReceiverInterfacePtr oldreceiver_stop = eventation->addEventReceiver( EVENT_ANIMATION_STOP, receiver );
        EventReceiverInterfacePtr oldreceiver_interrupt = eventation->addEventReceiver( EVENT_ANIMATION_INTERRUPT, receiver );

        MENGINE_ASSERTION_FATAL( oldreceiver_end == nullptr, "event EVENT_ANIMATION_END override" );
        MENGINE_ASSERTION_FATAL( oldreceiver_stop == nullptr, "event EVENT_ANIMATION_STOP override" );
        MENGINE_ASSERTION_FATAL( oldreceiver_interrupt == nullptr, "event EVENT_ANIMATION_INTERRUPT override" );

        receiver->setGOAPNode( _node );

        m_receiver = receiver;

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

        animation->setLastFrame();
        animation->stop();
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlayWait::_onFinally()
    {
        EventationInterface * eventation = m_eventable->getEventation();

        EventReceiverInterfacePtr delreceiver_end = eventation->removeEventReceiver( EVENT_ANIMATION_END );
        EventReceiverInterfacePtr delreceiver_stop = eventation->removeEventReceiver( EVENT_ANIMATION_STOP );
        EventReceiverInterfacePtr delreceiver_interrupt = eventation->removeEventReceiver( EVENT_ANIMATION_INTERRUPT );

        MENGINE_ASSERTION_FATAL( m_receiver == delreceiver_end, "event EVENT_ANIMATION_END miss remove" );
        MENGINE_ASSERTION_FATAL( m_receiver == delreceiver_stop, "event EVENT_ANIMATION_STOP miss remove" );
        MENGINE_ASSERTION_FATAL( m_receiver == delreceiver_interrupt, "event EVENT_ANIMATION_INTERRUPT miss remove" );

        m_receiver = nullptr;
        m_animatable = nullptr;
        m_eventable = nullptr;
    }
}