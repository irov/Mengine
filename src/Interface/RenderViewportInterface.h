#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/Viewport.h"
#include "Kernel/Resolution.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderViewportInterface
        : public Mixin
    {
    public:
        virtual const Viewport & getViewportWM() const = 0;

    public:
        virtual const Viewport & getGameViewport() const = 0;
        virtual const Resolution & getContentResolution() const = 0;

    public:
        virtual void fromScreenToViewportPosition( const mt::vec2f & _cameraPosition, mt::vec2f * const _viewportPosition ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderViewportInterface> RenderViewportInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}