#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/Resolution.h"
#include "Kernel/Viewport.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderResolutionInterface
        : public Mixin
    {
    public:
        virtual void setContentResolution( const Resolution & _resolution ) = 0;
        virtual const Resolution & getContentResolution() const = 0;

        virtual void setGameViewport( const Viewport & _viewport ) = 0;
        virtual const Viewport & getGameViewport() const = 0;

    public:
        virtual void fromContentToScreenPosition( const mt::vec2f & _contentPosition, mt::vec2f * const _screenPosition ) const = 0;
        virtual void fromScreenToContentPosition( const mt::vec2f & _screenPosition, mt::vec2f * const _contentPosition ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderResolutionInterface> RenderResolutionInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}