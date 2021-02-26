#pragma once

#include "OzzAnimationInterface.h"

#include "Kernel/Resource.h"

#include "ozz/animation/runtime/animation.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceOzzAnimation
        : public Resource
        , public UnknownResourceOzzAnimationInterface
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_UNKNOWABLE();

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
    typedef IntrusiveResourcePtr<ResourceOzzAnimation> ResourceOzzAnimationPtr;
    //////////////////////////////////////////////////////////////////////////
}