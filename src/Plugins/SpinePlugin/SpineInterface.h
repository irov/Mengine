#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/AnimationEventReceiver.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ESpineEventFlag
    {
        EVENT_SPINE_EVENT = __EVENT_ANIMATION_LAST__,
        EVENT_SPINE_STATE_ANIMATION_END,
        __EVENT_SPINE_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class SpineEventReceiver
        : public AnimationEventReceiver
    {
    public:
        virtual void onSpineEvent( const Char * _eventName, int32_t _eventIntValue, float _eventFloatValue, const Char * _eventStringValue ) = 0;
        virtual void onSpineStateAnimationEnd( const ConstString & _state, const ConstString & _animation, bool _isEnd ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SpineEventReceiver> SpineEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceSpineSkeleton
        : public UnknownInterface
    {
    public:
    };
}