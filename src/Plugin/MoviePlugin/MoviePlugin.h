#	pragma once

#	include "Core/PluginBase.h"

#	include "movie/movie.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class MoviePlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "Movie" )

	public:
		MoviePlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;

	protected:
		aeMovieInstance * m_instance;
	};
}