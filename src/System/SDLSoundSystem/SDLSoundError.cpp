#	include "SDLSoundError.h"

#	include "Logger/Logger.h"

#	include "SDL.h"

namespace Menge
{
	bool sdlSoundErrorCheck( ServiceProviderInterface * _serviceProvider, const char * _file, int _line )
	{
		//const char * err = SDL_GetError();

  //      if( err == S3E_SOUND_ERR_NONE )
  //      {
  //          return false;
  //      }

  //      const char * err_str = s3eSoundGetErrorString();

  //      LOGGER_ERROR( _serviceProvider )("Marmalade sound: %s:%d error %s:%d"
  //          , _file
  //          , _line
  //          , err_str
  //          , err
  //          );

        return true;
	}
}
