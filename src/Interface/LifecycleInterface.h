#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    class LifecycleInterface
        : public Mixin
    {
    public:
        virtual void preUpdate() = 0;
        virtual void update() = 0;
        virtual void postUpdate() = 0;
    };
}