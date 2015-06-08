#	pragma once

#   include "Interface/ServiceInterface.h"

namespace Menge
{
	bool SDLSoundErrorCheck( ServiceProviderInterface * _serviceProvider, const char * _file, int _line );
}

#	define SDL_SOUND_CHECK_ERROR(serviceProvider) Menge::SDLSoundErrorCheck( serviceProvider, __FILE__, __LINE__ )

