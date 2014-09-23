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
        void setEvent( PyObject * _cb );

    public:
        void setResourceMovie( ResourceMovie * _resourceMovie );

    protected:
        void _movieForwardUpdate( float _time, uint32_t _beginFrame, uint32_t _endFrame, const MovieLayer & _layer ) override;        

    protected:
        PyObject * m_cb;

        ResourceMovie * m_resourceMovie;
	};
}