#pragma once

#include "OzzAnimationInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/IntrusiveDerivedPtr.h"

#include "ozz/animation/runtime/skeleton.h"

namespace Mengine
{
    class ResourceOzzSkeleton
        : public Resource
        , public UnknownResourceOzzSkeletonInterface
    {
        DECLARE_UNKNOWABLE();

    public:
        ResourceOzzSkeleton();
        ~ResourceOzzSkeleton() override;

    public:
        void setFilePath( const FilePath & _filePath ) override;
        const FilePath & getFilePath() const override;

    public:
        const ozz::animation::Skeleton & getOzzSkeleton() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        FilePath m_filePath;

        ozz::animation::Skeleton m_skeleton;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveDerivedPtr<ResourceOzzSkeleton, Resource> ResourceOzzSkeletonPtr;
}