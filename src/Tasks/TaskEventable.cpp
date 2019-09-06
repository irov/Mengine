#pragma once

#include "TaskEventable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskEventable::~TaskEventable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskEventable::_onRun()
    {
        EventationInterface * eventation = m_eventable->getEventation();

        if( eventation == nullptr )
        {
            return true;
        }

        if( eventation->hasEventReceiver( m_event ) == false )
        {
            return true;
        }

        const EventReceiverInterfacePtr & reciever = eventation->getEventReciever( m_event );

        m_invoke( reciever.get() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskEventable::_onFinally()
    {
        m_invoke = nullptr;
    }
}