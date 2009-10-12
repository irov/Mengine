#	include "OALError.h"

#	include "OALSoundSystem.h"

namespace Menge
{
	void s_OALErrorCheck( OALSoundSystem * _system, const char * _file, int _line )
	{
		ALenum error = alGetError();

		if( error != AL_NO_ERROR )
		{
			const char * message = alGetString( error );

			_system->log_error( "OAL Error: (%s %d) %s\n"
				, _file
				, _line
				, message
				);
		}
	}
}
