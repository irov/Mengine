#pragma once

#include "Core/PluginBase.h"

#include "movie/movie.hpp"

#include "Movie2.h"
#include "ResourceMovie2.h"
#   include "Movie2Slot.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class MoviePlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "Movie" )

	public:
		MoviePlugin();

	protected:
		bool _avaliable() override;

	protected:
		bool _initialize() override;
		void _finalize() override;

    protected:
        PyObject * Movie2_setEventListener( pybind::kernel_interface * _kernel, Movie2 * _node, PyObject * _args, PyObject * _kwds );

	protected:
		const aeMovieInstance * m_instance;

		String m_hashkey;
	};
}