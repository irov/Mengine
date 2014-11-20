#	include "MarmaladeSoundError.h"

#	include "Logger/Logger.h"

#	include <s3eSound.h>

namespace Menge
{
	bool marmaladeSoundErrorCheck( ServiceProviderInterface * _serviceProvider, const char * _file, int _line )
	{
		s3eSoundError err = s3eSoundGetError();

		if( err == S3E_SOUND_ERR_NONE )
		{
			return false;
		}

		const char * err_str = s3eSoundGetErrorString();

		LOGGER_ERROR(_serviceProvider)("Marmalade sound: %s:%d error %s:%d"
			, _file
			, _line
			, err_str
			, err 
			);

        return true;
	}
}
