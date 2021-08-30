#include "TaskEventable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskEventable::~TaskEventable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskEventable::_onRun( GOAP::NodeInterface * _node )
    {
        MENGINE_UNUSED( _node );

        EventationInterface * eventation = m_eventable->getEventation();

        if( eventation == nullptr )
        {
            return true;
        }

        if( eventation->hasEventReceiver( m_event ) == false )
        {
            return true;
        }

        const EventReceiverInterfacePtr & receiver = eventation->getEventReceiver( m_event );

        m_invoke( receiver.get() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskEventable::_onFinally()
    {
        m_invoke = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}