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
        const Viewport & getScissorViewportWM() const override;

    protected:
        Viewport m_viewport;
        Viewport m_viewportWM;
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
