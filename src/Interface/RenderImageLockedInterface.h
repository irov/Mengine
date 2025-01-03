#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/Pointer.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderImageLockedInterface
        : public Mixin
    {
    public:
        virtual Pointer getBuffer( size_t * const _pitch ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderImageLockedInterface> RenderImageLockedInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
