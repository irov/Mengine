#	pragma once

#	include "Interface/ServiceInterface.h"

#   include "Config/Typedef.h"
#   include "Config/String.h"

namespace Menge
{
	class CriticalErrorsMonitor
	{
	public:
		static void run( const WString & _dumpPath, ServiceProviderInterface * _serviceProvider );
		static void stop();
	};
}
