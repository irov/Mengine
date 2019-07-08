#pragma once

#include "Kernel/Factorable.h"

#include "Interface/RenderScissorInterface.h"

#include "movie/movie.hpp"

#include "Config/Typedef.h"

namespace Mengine
{
    class Movie2Scissor
        : public Factorable
        , public RenderScissorInterface
    {
    public:
        Movie2Scissor();
        ~Movie2Scissor() override;

    public:
        void setViewport( const mt::mat4f & _wm, const ae_viewport_t * _viewport );

    public:
        const Viewport & getScissorViewport() const override;

    protected:
        Viewport m_viewport;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie2Scissor> Movie2ScissorPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const Viewport & Movie2Scissor::getScissorViewport() const
    {
        return m_viewport;
    }
}
