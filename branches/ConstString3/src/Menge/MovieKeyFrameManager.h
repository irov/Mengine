#	pragma once

#	include <vector>
#	include "Config/Typedef.h"
#	include <Core/Holder.h>
#	include "Utils/Core/ConstString.h"
#	include "MovieFramePack.h"

namespace Menge
{
	class BinParser;
	
	typedef std::vector<MovieFramePack *> TVectorMovieFramePack;
	
	class MovieKeyFrameManager
		: public Holder<MovieKeyFrameManager>
	{
	public:
		MovieKeyFrameManager();
		~MovieKeyFrameManager();

		MovieFramePack * getMovieFramePak( const ConstString & _pak, const WString & _path );
		void releaseMovieFramePak( MovieFramePack * _framePak );
		void loaderMovieFramePak( BinParser * _parser, MovieFramePack * _framePak );

	protected:
		MovieFramePack * createMovieFramePak_( const ConstString & _pak, const WString & _path );
		void loaderKeyFrames2D_( BinParser * _parser, size_t _layerIndex, MovieFramePack * _framePak );
		void loaderKeyFrames3D_( BinParser * _parser, size_t _layerIndex, MovieFramePack * _framePak );
		TVectorMovieFramePack m_framePacks;
		size_t m_curLayer;
	};
}