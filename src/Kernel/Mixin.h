#pragma once

#include "Kernel/IntrusivePtrBase.h"

namespace Mengine
{
    class Mixin
        : virtual public IntrusivePtrBase
    {
    public:
        Mixin() noexcept
        {
        }

        ~Mixin() override
        {
        }
    };
}