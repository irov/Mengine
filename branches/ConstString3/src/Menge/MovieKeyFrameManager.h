#	pragma once

#	include <vector>
#	include "Config/Typedef.h"
#	include <Core/Holder.h>
#	include "Utils/Core/ConstString.h"
#	include "MovieFramePack.h"

namespace Menge
{
	class MovieKeyFrameManager
		: public Holder<MovieKeyFrameManager>
	{
	public:
		MovieKeyFrameManager();
		~MovieKeyFrameManager();

	public:
		MovieFramePack * getMovieFramePak( const ConstString & _pak, const WString & _path );
		void releaseMovieFramePak( MovieFramePack * _framePak );

	protected:
		MovieFramePack * createMovieFramePak_( const ConstString & _pak, const WString & _path );
	};
}