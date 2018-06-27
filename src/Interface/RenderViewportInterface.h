#pragma once

#include "Core/Mixin.h"
#include "Core/IntrusivePtr.h"

#include "Core/Viewport.h"

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