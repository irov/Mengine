#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/FilePath.h"
#include "Kernel/Resource.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/Animatable.h"
#include "Kernel/Eventable.h"
#include "Kernel/Updatable.h"
#include "Kernel/Compilable.h"

#include "Kernel/AnimationEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum NodeOzzAnimationEventFlag
    {
        __EVENT_NODE_OZZ_ANIMATION_LAST__ = __EVENT_ANIMATION_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class SamplerOzzAnimationEventReceiver
        : public AnimationEventReceiver
    {
    public:
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SamplerOzzAnimationEventReceiver> SamplerOzzAnimationEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceOzzAnimationInterface
        : public UnknownInterface
    {
    public:
        virtual void setFilePath( const FilePath & _filePath ) = 0;
        virtual const FilePath & getFilePath() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownResourceOzzAnimationInterface> UnknownResourceOzzAnimationInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceOzzSkeletonInterface
        : public UnknownInterface
    {
    public:
        virtual void setFilePath( const FilePath & _filePath ) = 0;
        virtual const FilePath & getFilePath() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownResourceOzzSkeletonInterface> UnknownResourceOzzSkeletonInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceOzzMeshInterface
        : public UnknownInterface
    {
    public:
        virtual void setFilePath( const FilePath & _filePath ) = 0;
        virtual const FilePath & getFilePath() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownResourceOzzMeshInterface> UnknownResourceOzzMeshInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SamplerOzzAnimationInterface
        : public Servant
        , public Compilable
        , public Updatable
        , public Animatable
        , public Eventable
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
        virtual void addOzzAnimationSampler( const ConstString & _name, const SamplerOzzAnimationInterfacePtr & _sampler ) = 0;
        virtual void removeOzzAnimationSampler( const ConstString & _name ) = 0;

        virtual const SamplerOzzAnimationInterfacePtr & findOzzAnimationSampler( const ConstString & _name ) const = 0;

        virtual uint32_t getOzzAnimationSamplerCount() const = 0;
        virtual const SamplerOzzAnimationInterfacePtr & getOzzAnimationSampler( uint32_t _index ) const = 0;        
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownNodeOzzAnimationInterface> UnknownNodeOzzAnimationInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
