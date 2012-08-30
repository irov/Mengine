#	include "OALError.h"

#	include "OALSoundSystem.h"

#	include "Logger/Logger.h"

namespace Menge
{
	bool s_OALErrorCheck( OALSoundSystem * _soundSystem, const char * _file, int _line )
	{
		ALenum error = alGetError();

		if( error == AL_NO_ERROR )
		{
			return false;
		}

		const char * message = alGetString( error );

		LogServiceInterface * logService = _soundSystem->getLogService();

		LOGGER_ERROR(logService)( "OAL Error: (%s %d) %d:%s"
			, _file
			, _line
            , error
			, message
			);	

        return true;
	}
}
