#pragma once

#include "Kernel/IntrusivePtrBase.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Mixin
        : virtual public IntrusivePtrBase
    {
    public:
        Mixin() MENGINE_NOEXCEPT
        {
        }

        virtual ~Mixin() MENGINE_NOEXCEPT
        {
        }
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Mixin> MixinPtr;
    //////////////////////////////////////////////////////////////////////////
}