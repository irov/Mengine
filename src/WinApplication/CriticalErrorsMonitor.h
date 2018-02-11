#	pragma once

#   include "Config/Typedef.h"
#   include "Config/String.h"

namespace Menge
{
	class CriticalErrorsMonitor
	{
	public:
		static void run( const WString & _dumpPath );
		static void stop();
	};
}
