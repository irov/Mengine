#pragma once

#include "Interface/AnimationInterface.h"

#include "Config/Typedef.h"

#include "Kernel/IntrusivePtr.h"
#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Animatable
        : public Mixin
    {
    public:
        virtual AnimationInterfacePtr getAnimation()
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
    Mengine::AnimationInterfacePtr getAnimation() override{ return this; }\
protected:
