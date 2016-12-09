#	pragma once

#   include "Factory/FactorablePtr.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class EventReceiver
        : public FactorablePtr
    { 
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<EventReceiver> EventReceiverPtr;
}