#pragma once

#   include "Factory/FactorablePtr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class EventReceiver
        : public FactorablePtr
    { 
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<EventReceiver> EventReceiverPtr;
}