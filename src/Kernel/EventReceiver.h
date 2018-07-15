#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class EventReceiver
        : public Factorable
    { 
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EventReceiver> EventReceiverPtr;
}