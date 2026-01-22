#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/Rect.h"
#include "Kernel/Pointer.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderImageLockedInterface
        : public Mixin
    {
    public:
        virtual const Rect & getLockedRect() const = 0;

    public:
        virtual Pointer getLockedBuffer( size_t * const _pitch ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderImageLockedInterface> RenderImageLockedInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
