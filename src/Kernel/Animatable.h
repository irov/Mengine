#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnimationInterface;
    //////////////////////////////////////////////////////////////////////////
    class Animatable
        : public Mixin
    {
    public:
        virtual AnimationInterface * getAnimation()
        {
            return nullptr;
        };

        virtual const AnimationInterface * getAnimation() const
        {
            return nullptr;
        };
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Animatable> AnimatablePtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_ANIMATABLE()\
public:\
    Mengine::AnimationInterface * getAnimation() override{ return this; }\
    const Mengine::AnimationInterface * getAnimation() const override{ return this; }\
    Mengine::Animatable * getAnimatable() override{ return this; }\
protected:
//////////////////////////////////////////////////////////////////////////