#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/Viewport.h"
#include "Kernel/Resolution.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderScissorInterface
        : public Mixin
    {
    public:
        virtual const Viewport & getScissorViewportWM() const = 0;

    public:
        virtual const Viewport & getGameViewport() const = 0;
        virtual const Resolution & getContentResolution() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderScissorInterface> RenderScissorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}