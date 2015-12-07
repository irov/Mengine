#	pragma once

#	include "Kernel/Node.h"

#   include "MovieNodeExtra.h"

namespace Menge
{
	class MovieEvent
		: public MovieNodeExtra
	{
    public:
        MovieEvent();
        ~MovieEvent();

    public:
		void setEvent( const pybind::object & _cb );

    public:
        void setResourceMovie( const ResourceMoviePtr & _resourceMovie );

    protected:
        void _movieForwardUpdate( float _time, uint32_t _beginFrame, uint32_t _endFrame, const MovieLayer & _layer ) override;        

    protected:
		pybind::object m_cb;

		ResourceMoviePtr m_resourceMovie;
	};
}