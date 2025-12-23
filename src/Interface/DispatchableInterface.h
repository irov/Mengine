#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    class DispatchableInterface
        : public Mixin
    {
    public:
        virtual void preUpdate() = 0;
        virtual void update() = 0;
        virtual void postUpdate() = 0;
    };
}
