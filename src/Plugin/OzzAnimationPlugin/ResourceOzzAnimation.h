#pragma once

#include "OzzAnimationInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/IntrusiveDerivedPtr.h"

#include "ozz/animation/runtime/animation.h"

namespace Mengine
{
    class ResourceOzzAnimation
        : public Resource
        , public UnknownResourceOzzAnimationInterface
    {
        DECLARE_UNKNOWABLE();

    public:
        ResourceOzzAnimation();
        ~ResourceOzzAnimation() override;

    public:
        void setFilePath( const FilePath & _filePath ) override;
        const FilePath & getFilePath() const override;

    public:
        const ozz::animation::Animation & getOzzAnimation() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        FilePath m_filePath;

        ozz::animation::Animation m_animation;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveDerivedPtr<ResourceOzzAnimation, Resource> ResourceOzzAnimationPtr;
}