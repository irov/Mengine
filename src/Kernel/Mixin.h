#pragma once

#include "Config/Config.h"

#include "Kernel/IntrusivePtrBase.h"
#include "Kernel/IntrusivePtr.h"

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
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Mixin> MixinPtr;
    //////////////////////////////////////////////////////////////////////////
}