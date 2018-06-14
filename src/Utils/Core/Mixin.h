#pragma once

#include "Core/IntrusivePtrBase.h"

namespace Mengine
{
    class Mixin
        : virtual public IntrusivePtrBase
    {
    public:
        Mixin()
        {
        }

        ~Mixin() override
        {
        }
    };
}