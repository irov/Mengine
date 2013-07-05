#	pragma once

#   include "Config/Typedef.h"
#   include "Config/String.h"

namespace Menge
{
	class CriticalErrorsMonitor
	{
	public:
		static void run( const char * _versionInfo, const WString & _logPath, const WString & _dumpPath );
	};
}
