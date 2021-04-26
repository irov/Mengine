#pragma once

#include "Interface/EventReceiverInterface.h"

#include "AreaOfInterestInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum NodeAreaOfInterestTriggerEventFlag
    {
        EVENT_AREAOFINTEREST_TRIGGER_ENTER = 0,
        EVENT_AREAOFINTEREST_TRIGGER_LEAVE,
        
        __EVENT_AREAOFINTEREST_TRIGGER_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class NodeAreaOfInterestTriggerEventReceiverInterface
        : public EventReceiverInterface
    {
    public:
        virtual void onAreaOfInterestTriggerEnter( const AreaOfInterestActorProviderInterfacePtr & _enemy, uint32_t _iff1, uint32_t _iff2 ) = 0;
        virtual void onAreaOfInterestTriggerLeave( const AreaOfInterestActorProviderInterfacePtr & _enemy, uint32_t _iff1, uint32_t _iff2 ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( NodeAreaOfInterestTriggerEventReceiverInterface, EVENT_AREAOFINTEREST_TRIGGER_ENTER );
    EVENTATION_TYPEID( NodeAreaOfInterestTriggerEventReceiverInterface, EVENT_AREAOFINTEREST_TRIGGER_LEAVE );
    //////////////////////////////////////////////////////////////////////////
}