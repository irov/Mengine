#pragma once

#include "OzzAnimationInterface.h"

#include "Kernel/Resource.h"

#include "ozz/animation/runtime/skeleton.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceOzzSkeleton
        : public Resource
        , public UnknownResourceOzzSkeletonInterface
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_UNKNOWABLE();

    public:
        ResourceOzzSkeleton();
        ~ResourceOzzSkeleton() override;

    public:
        const ozz::animation::Skeleton & getOzzSkeleton() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        ozz::animation::Skeleton m_skeleton;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceOzzSkeleton> ResourceOzzSkeletonPtr;
    //////////////////////////////////////////////////////////////////////////
}