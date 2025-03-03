#pragma once

#include "Interface/RenderResolutionInterface.h"

#include "Kernel/Viewport.h"
#include "Kernel/Resolution.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderResolution
        : public RenderResolutionInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( RenderResolution );

    public:
        RenderResolution();
        ~RenderResolution() override;

    public:
        void setContentResolution( const Resolution & _resolution ) override;
        const Resolution & getContentResolution() const override;

        void setGameViewport( const Viewport & _viewport ) override;
        const Viewport & getGameViewport() const override;

    public:
        void fromContentToScreenPosition( const mt::vec2f & _contentPosition, mt::vec2f * const _screenPosition ) const override;
        void fromScreenToContentPosition( const mt::vec2f & _screenPosition, mt::vec2f * const _contentPosition ) const override;

    protected:
        Resolution m_contentResolution;
        Viewport m_gameViewport;

        mt::vec2f m_contentResolutionSize;
        mt::vec2f m_contentResolutionSizeInv;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderResolution, RenderResolutionInterface> RenderResolutionPtr;
    //////////////////////////////////////////////////////////////////////////
}
