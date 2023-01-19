#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/UnknownInterface.h"

#include "Kernel/FilePath.h"
#include "Kernel/Resource.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/Animatable.h"
#include "Kernel/Eventable.h"
#include "Kernel/Updatable.h"
#include "Kernel/Compilable.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "Kernel/Scriptable.h"
#endif

#include "Kernel/AnimationEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ENodeOzzAnimationEventFlag
    {
        __EVENT_NODE_OZZ_ANIMATION_LAST__ = __EVENT_ANIMATION_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class SamplerOzzAnimationEventReceiverInterface
        : public AnimationEventReceiverInterface
    {
    public:
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SamplerOzzAnimationEventReceiverInterface> SamplerOzzAnimationEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceOzzAnimationInterface
        : public UnknownInterface
    {
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownResourceOzzAnimationInterface> UnknownResourceOzzAnimationInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceOzzSkeletonInterface
        : public UnknownInterface
    {
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownResourceOzzSkeletonInterface> UnknownResourceOzzSkeletonInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceOzzMeshInterface
        : public UnknownInterface
    {
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownResourceOzzMeshInterface> UnknownResourceOzzMeshInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SamplerOzzAnimationInterface
        : public ServantInterface
        , public Identity
        , public Compilable
        , public Updatable
        , public Animatable
        , public Eventable
#ifdef MENGINE_USE_SCRIPT_SERVICE
        , public Scriptable
#endif
    {
    public:
        virtual void setResourceOzzAnimation( const ResourcePtr & _resource ) = 0;
        virtual const ResourcePtr & getResourceOzzAnimation() const = 0;

        virtual void setResourceOzzSkeleton( const ResourcePtr & _resource ) = 0;
        virtual const ResourcePtr & getResourceOzzSkeleton() const = 0;

        virtual void setWeight( float _weight ) = 0;
        virtual float getWeight() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SamplerOzzAnimationInterface> SamplerOzzAnimationInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownNodeOzzAnimationInterface
        : public UnknownInterface
    {
    public:
        virtual void setResourceOzzImage( const ResourceImagePtr & _resource ) = 0;
        virtual const ResourceImagePtr & getResourceOzzImage() const = 0;

        virtual void setResourceOzzSkeleton( const ResourcePtr & _resource ) = 0;
        virtual const ResourcePtr & getResourceOzzSkeleton() const = 0;

        virtual void setResourceOzzMesh( const ResourcePtr & _resource ) = 0;
        virtual const ResourcePtr & getResourceOzzMesh() const = 0;

    public:
        virtual void addOzzAnimationSampler( const SamplerOzzAnimationInterfacePtr & _sampler ) = 0;
        virtual void removeOzzAnimationSampler( const ConstString & _name ) = 0;

        virtual const SamplerOzzAnimationInterfacePtr & findOzzAnimationSampler( const ConstString & _name ) const = 0;

        virtual uint32_t getOzzAnimationSamplerCount() const = 0;
        virtual const SamplerOzzAnimationInterfacePtr & getOzzAnimationSampler( uint32_t _index ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownNodeOzzAnimationInterface> UnknownNodeOzzAnimationInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
