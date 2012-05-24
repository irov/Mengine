#	include "MovieKeyFrameManager.h"
#	include "LogEngine.h"
#	include "BinParser.h"
#	include "Loadable.h"
#	include "LoaderEngine.h"

#	include "LoaderMovieKeyFrame.h"

namespace Menge
{

	////////////////////////////////////////////////////////////////////////////////////////////
	MovieKeyFrameManager::MovieKeyFrameManager()
	{
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	MovieKeyFrameManager::~MovieKeyFrameManager()
	{
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	MovieFramePack * MovieKeyFrameManager::getMovieFramePak( const ConstString & _pak, const WString & _path )
	{
		MovieFramePack * framePak = this->createMovieFramePak_( _pak, _path );
		 
		if( framePak == NULL )
		{
			MENGE_LOG_ERROR( "MovieKeyFrameManager: can`t load file category %s path %S"
				, _pak.c_str()
				, _path.c_str()
				);

			return NULL;
		}

		return framePak;
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	MovieFramePack * MovieKeyFrameManager::createMovieFramePak_( const ConstString & _pak, const WString & _path )
	{
		bool exist = false;
		
		LoaderMovieKeyFrame loadable;

		if( LoaderEngine::get()
			->load( _pak, _path, &loadable, exist ) == false )
		{
			if( exist == false )
			{
				MENGE_LOG_ERROR( "MovieKeyFrameManager: KeyFramesFile '%S' not found"
					, _path.c_str()
					);
			}
			else
			{
				MENGE_LOG_ERROR( "MovieKeyFrameManager: KeyFramesFile invalid parse '%S' "
					, _path.c_str()
					);
			}

			return NULL;
		}

		MovieFramePack * pack = loadable.getFramePack();

		return pack;
	}
	////////////////////////////////////////////////////////////////////////////////
	void MovieKeyFrameManager::releaseMovieFramePak( MovieFramePack * _framePak )
	{
		delete _framePak;
	}
	////////////////////////////////////////////////////////////////////////////////
}
