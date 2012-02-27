#	include "OALError.h"

#	include "OALSoundSystem.h"

#	include "Logger/Logger.h"

namespace Menge
{
	void s_OALErrorCheck( OALSoundSystem * _soundSystem, const char * _file, int _line )
	{
		ALenum error = alGetError();

		if( error == AL_NO_ERROR )
		{
			return;
		}

		const char * message = alGetString( error );

		LogSystemInterface * logSystem = _soundSystem->getLogSystem();

		LOGGER_ERROR(logSystem)( "OAL Error: (%s %d) %s"
			, _file
			, _line
			, message
			);	
	}
}
