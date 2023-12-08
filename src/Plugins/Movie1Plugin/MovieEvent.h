#pragma once

#include "Environment/Python/PythonIncluder.h"

#include "MovieNodeExtra.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MovieEvent
        : public MovieNodeExtra
    {
        DECLARE_FACTORABLE( MovieNodeExtra );
        DECLARE_VISITABLE( Node );

    public:
        MovieEvent();
        ~MovieEvent() override;

    public:
        void setEvent( const pybind::object & _cb, const pybind::args & _args );
        void removeEvent();

    public:
        void setResourceMovie( const ResourceMoviePtr & _resourceMovie );

    protected:
        void _movieForwardUpdate( float _time, uint32_t _beginFrame, uint32_t _endFrame, const MovieLayer & _layer ) override;

    protected:
        pybind::object m_cb;
        pybind::args m_args;

        ResourceMoviePtr m_resourceMovie;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MovieEvent> MovieEventPtr;
    //////////////////////////////////////////////////////////////////////////
}