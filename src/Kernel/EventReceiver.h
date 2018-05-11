#pragma once

#include "Factory/Factorable.h"
#include "Core/IntrusivePtr.h"

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