#pragma once

#include "Interface/EventReceiverInterface.h"

#include "Kernel/AnimationEventReceiver.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EMovie2EventFlag
    {
        EVENT_MOVIE2_MARKER = __EVENT_ANIMATION_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class Movie2MarkerEventReceiverInterface
        : public EventReceiverInterface
    {
    public:
        virtual void onMovieMarker( const ConstString & _name, float _time, uint32_t _iteration ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( Movie2MarkerEventReceiverInterface, EVENT_MOVIE2_MARKER );
    //////////////////////////////////////////////////////////////////////////
}
