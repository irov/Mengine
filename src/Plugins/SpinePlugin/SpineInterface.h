#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/AnimationEventReceiver.h"
#include "Kernel/Resource.h"

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
    //////////////////////////////////////////////////////////////////////////
    class UnknownSpine
        : public UnknownInterface
    {
    public:
        virtual void setResourceSpineSkeleton( const ResourcePtr & _resourceSpineSkeleton ) = 0;
        virtual const ResourcePtr & getResourceSpineSkeleton() const = 0;

    public:
        virtual bool mixAnimation( const ConstString & _first, const ConstString & _second, float _mix ) = 0;

    public:
        virtual bool setStateAnimation( const ConstString & _state, const ConstString & _name, float _timing, float _speedFactor, bool _loop ) = 0;
        virtual bool removeStateAnimation( const ConstString & _state ) = 0;

    public:
        virtual bool setStateAnimationSpeedFactor( const ConstString & _state, float _speedFactor ) = 0;
        virtual float getStateAnimationSpeedFactor( const ConstString & _state ) const = 0;

        virtual bool setStateAnimationTiming( const ConstString & _state, float _timing ) = 0;
        virtual float getStateAnimationTiming( const ConstString & _state ) const = 0;

        virtual bool setStateAnimationFreeze( const ConstString & _state, bool _freeze ) = 0;
        virtual bool getStateAnimationFreeze( const ConstString & _state ) const = 0;

        virtual float getStateAnimationDuration( const ConstString & _state ) const = 0;

    public:
        virtual float getAnimationDuration( const ConstString & _name ) = 0;
    };
}