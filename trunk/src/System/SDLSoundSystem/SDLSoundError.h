#	pragma once

#   include "Interface/ServiceInterface.h"

namespace Menge
{
	bool marmaladeSoundErrorCheck( ServiceProviderInterface * _serviceProvider, const char * _file, int _line );
}

#	define MARMALADE_SOUND_CHECK_ERROR(serviceProvider) Menge::marmaladeSoundErrorCheck( serviceProvider, __FILE__, __LINE__ )

