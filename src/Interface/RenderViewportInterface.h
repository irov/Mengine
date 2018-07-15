#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

#include "Kernel/Viewport.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderViewportInterface
        : public Mixin
    {
    public:
        virtual const Viewport & getViewport() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderViewportInterface> RenderViewportInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}