#	pragma once

#   include "Interface/ServiceInterface.h"

namespace Menge
{
	bool s_OALErrorCheck( ServiceProviderInterface * _serviceProvider, const char * _file, int _line );
}

#	define OAL_CHECK_ERROR(serviceProvider) Menge::s_OALErrorCheck( serviceProvider, __FILE__, __LINE__ )

