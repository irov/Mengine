#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/AnimationEventReceiver.h"
#include "Kernel/Resource.h"
#include "Kernel/Updatable.h"
#include "Kernel/Animatable.h"
#include "Kernel/Eventable.h"

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
    class SpineEventReceiverInterface
        : public AnimationEventReceiverInterface
    {
    public:
        virtual void onSpineEvent( const Char * _eventName, int32_t _eventIntValue, float _eventFloatValue, const Char * _eventStringValue ) = 0;
        virtual void onSpineStateAnimationEnd( const ConstString & _stateName, const ConstString & _animationName, bool _isEnd ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SpineEventReceiverInterface> SpineEventReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( SpineEventReceiverInterface, EVENT_SPINE_EVENT );
    EVENTATION_TYPEID( SpineEventReceiverInterface, EVENT_SPINE_STATE_ANIMATION_END );
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceSpineAtlasTexturepackerInterface
        : public UnknownInterface
    {
    public:
        virtual void addResourceTexturepacker( const ResourcePtr & _resourceTexturepacker ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceSpineSkeletonInterface
        : public UnknownInterface
    {
    public:
        virtual void setResourceSpineAtlas( const ResourcePtr & _resourceSpineAtlas ) = 0;
        virtual const ResourcePtr & getResourceSpineAtlas() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class SamplerSpineAnimationInterface
        : public ServantInterface
        , public Identity
        , public Compilable
        , public Updatable
        , public Animatable
        , public Eventable
        , public Scriptable
    {
    public:
        virtual void setResourceSpineSkeleton( const ResourcePtr & _resourceSpineSkeleton ) = 0;
        virtual const ResourcePtr & getResourceSpineSkeleton() const = 0;

    public:
        virtual void setAnimationStateName( const ConstString & _animationStateName ) = 0;
        virtual const ConstString & getAnimationStateName() const = 0;

    public:
        virtual void setAnimationName( const ConstString & _animationName ) = 0;
        virtual const ConstString & getAnimationName() const = 0;

    public:
        virtual void setSpeedFactor( float _speedFactor ) = 0;
        virtual float getSpeedFactor() const = 0;

        virtual void setTime( float _time ) = 0;
        virtual float getTime() const = 0;

        virtual bool setFreeze( bool _freeze ) = 0;
        virtual bool getFreeze() const = 0;

        virtual float getDuration() const = 0;

        virtual bool setLoop( bool _loop ) = 0;
        virtual bool getLoop() const = 0;

        virtual bool setFirstFrame() = 0;
        virtual bool setLastFrame() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SamplerSpineAnimationInterface> SamplerSpineAnimationInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownSpineInterface
        : public UnknownInterface
    {
    public:
        virtual void setResourceSpineSkeleton( const ResourcePtr & _resourceSpineSkeleton ) = 0;
        virtual const ResourcePtr & getResourceSpineSkeleton() const = 0;

    //public:
    //    virtual void addSpineAnimationSampler( const SamplerSpineAnimationInterfacePtr & _sampler ) = 0;
    //    virtual void removeSpineAnimationSampler( const ConstString & _name ) = 0;

    //    virtual const SamplerSpineAnimationInterfacePtr & findSpineAnimationSampler( const ConstString & _name ) const = 0;

    //    virtual uint32_t getSpineAnimationSamplerCount() const = 0;
    //    virtual const SamplerSpineAnimationInterfacePtr & getSpineAnimationSampler( uint32_t _index ) const = 0;

    public:
        virtual bool mixAnimation( const ConstString & _first, const ConstString & _second, float _mix ) = 0;

    public:
        virtual bool setStateAnimation( const ConstString & _state, const ConstString & _name, float _time, float _speedFactor, bool _loop ) = 0;
        virtual bool removeStateAnimation( const ConstString & _state ) = 0;

        virtual bool hasSkeletonAnimationName( const ConstString & _name ) const = 0;

    public:
        virtual bool setStateAnimationSpeedFactor( const ConstString & _state, float _speedFactor ) = 0;
        virtual float getStateAnimationSpeedFactor( const ConstString & _state ) const = 0;

        virtual bool setStateAnimationTime( const ConstString & _state, float _time ) = 0;
        virtual float getStateAnimationTime( const ConstString & _state ) const = 0;

        virtual bool setStateAnimationFreeze( const ConstString & _state, bool _freeze ) = 0;
        virtual bool getStateAnimationFreeze( const ConstString & _state ) const = 0;

        virtual float getStateAnimationDuration( const ConstString & _state ) const = 0;

        virtual bool setStateAnimationLoop( const ConstString & _state, bool _loop ) = 0;
        virtual bool getStateAnimationLoop( const ConstString & _state ) const = 0;

    public:
        virtual bool setStateAnimationFirstFrame( const ConstString & _state, const ConstString & _name ) = 0;
        virtual bool setStateAnimationLastFrame( const ConstString & _state, const ConstString & _name ) = 0;
    };
}