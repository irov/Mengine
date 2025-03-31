#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/AnimationEventReceiver.h"
#include "Kernel/Resource.h"
#include "Kernel/Updatable.h"
#include "Kernel/Animatable.h"
#include "Kernel/Eventable.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Kernel/Scriptable.h"
#endif

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ESpineEventFlag
    {
        EVENT_SAMPLER_SPINE_EVENT = __EVENT_ANIMATION_LAST__,
        EVENT_SAMPLER_SPINE_STATE_ANIMATION_END,

        __EVENT_SPINE_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class SamplerSpineEventReceiverInterface
        : public AnimationEventReceiverInterface
    {
    public:
        virtual void onSamplerSpineEvent( const Char * _eventName, int32_t _eventIntValue, float _eventFloatValue, const Char * _eventStringValue ) = 0;
        virtual void onSamplerSpineStateAnimationEnd( const ConstString & _stateName, const ConstString & _animationName, bool _isEnd ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SamplerSpineEventReceiverInterface> SamplerSpineEventReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( SamplerSpineEventReceiverInterface, EVENT_SAMPLER_SPINE_EVENT );
    EVENTATION_TYPEID( SamplerSpineEventReceiverInterface, EVENT_SAMPLER_SPINE_STATE_ANIMATION_END );
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
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        , public Scriptable
#endif
    {
    public:
        virtual void setResourceSpineSkeleton( const ResourcePtr & _resourceSpineSkeleton ) = 0;
        virtual const ResourcePtr & getResourceSpineSkeleton() const = 0;

    public:
        virtual void setAnimationName( const ConstString & _animationName ) = 0;
        virtual const ConstString & getAnimationName() const = 0;

    public:
        virtual void setAnimationEnable( bool _enable ) = 0;
        virtual bool getAnimationEnable() const = 0;
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

    public:
        virtual bool addAnimationSampler( const SamplerSpineAnimationInterfacePtr & _sampler ) = 0;
        virtual void removeAnimationSampler( const ConstString & _name ) = 0;
        virtual void clearAnimationSamplers() = 0;

        virtual const SamplerSpineAnimationInterfacePtr & findAnimationSampler( const ConstString & _name ) const = 0;

        virtual uint32_t getAnimationSamplerCount() const = 0;
        virtual const SamplerSpineAnimationInterfacePtr & getAnimationSampler( uint32_t _index ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}