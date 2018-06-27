#pragma once

#include "Core/Mixin.h"
#include "Core/IntrusivePtr.h"

#include "Core/Viewport.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderScissorInterface
        : public Mixin
    {
    public:
        virtual const Viewport & getScissorViewport() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderScissorInterface> RenderScissorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}