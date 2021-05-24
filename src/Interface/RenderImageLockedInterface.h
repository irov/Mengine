#pragma once

#include "Interface/Interface.h"

#include "Kernel/Pointer.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderImageLockedInterface
        : public Interface
    {
    public:
        virtual Pointer getBuffer( size_t * const _pitch ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderImageLockedInterface> RenderImageLockedInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
