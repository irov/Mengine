#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class EventReceiverInterface
        : public Mixin
    {
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EventReceiverInterface> EventReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define EVENTATION_TYPEID(Type, Event)\
    typedef Type EventationTypeid_##Event;
//////////////////////////////////////////////////////////////////////////
#define EVENTATION_GETTYPE(Event)\
    EventationTypeid_##Event
//////////////////////////////////////////////////////////////////////////