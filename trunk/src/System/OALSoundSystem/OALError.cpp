#	include "OALError.h"

#   include <AL/al.h>

#	include "Logger/Logger.h"

namespace Menge
{
	bool s_OALErrorCheck( ServiceProviderInterface * _serviceProvider, const char * _file, int _line )
	{
		ALenum error = alGetError();

		if( error == AL_NO_ERROR )
		{
			return false;
		}

		const char * message = alGetString( error );

		LOGGER_ERROR(_serviceProvider)( "OAL Error: (%s %d) %d:%s"
			, _file
			, _line
            , error
			, message
			);	

        return true;
	}
}
