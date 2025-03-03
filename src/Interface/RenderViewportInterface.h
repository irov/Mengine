#pragma once

#include "Interface/RenderResolutionInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/Viewport.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderViewportInterface
        : public Mixin
    {
    public:
        virtual const Viewport & getViewportWM() const = 0;

    public:
        virtual void fromCameraToContentPosition( const mt::vec2f & _cameraPosition, mt::vec2f * const _contentPosition ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderViewportInterface> RenderViewportInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}