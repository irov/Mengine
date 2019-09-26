#pragma once

#include "OzzAnimationInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/BaseContent.h"

#include "ozz/animation/runtime/animation.h"

namespace Mengine
{
    class ResourceOzzAnimation
        : public Resource
        , private BaseContent
        , public UnknownResourceOzzAnimationInterface
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_UNKNOWABLE();
        DECLARE_CONTENTABLE();

    public:
        ResourceOzzAnimation();
        ~ResourceOzzAnimation() override;

    public:
        const ozz::animation::Animation & getOzzAnimation() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        ozz::animation::Animation m_animation;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceOzzAnimation, Resource> ResourceOzzAnimationPtr;
}