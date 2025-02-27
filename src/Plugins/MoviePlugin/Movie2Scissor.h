#pragma once

#include "Kernel/Factorable.h"

#include "Interface/RenderScissorInterface.h"

#include "movie/movie.hpp"

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Movie2Scissor
        : public Factorable
        , public RenderScissorInterface
    {
        DECLARE_FACTORABLE( Movie2Scissor );

    public:
        Movie2Scissor();
        ~Movie2Scissor() override;

    public:
        void setScissorViewport( const mt::mat4f & _wm, const ae_viewport_t * _viewport );
        const Viewport & getScissorViewport() const;

    public:
        void setGameViewport( const Viewport & _viewport );
        void setContentResolution( const Resolution & _resolution );

    public:
        const Viewport & getScissorViewportWM() const override;

    public:
        const Viewport & getGameViewport() const override;
        const Resolution & getContentResolution() const override;

    protected:
        Viewport m_viewport;
        Viewport m_viewportWM;

        Viewport m_gameViewport;

        mt::vec2f m_gameViewportSize;
        mt::vec2f m_gameViewportSizeInv;

        Resolution m_contentResolution;

        mt::vec2f m_contentResolutionSize;
        mt::vec2f m_contentResolutionSizeInv;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie2Scissor> Movie2ScissorPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const Viewport & Movie2Scissor::getScissorViewport() const
    {
        return m_viewport;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const Viewport & Movie2Scissor::getScissorViewportWM() const
    {
        return m_viewportWM;
    }
    //////////////////////////////////////////////////////////////////////////
}
