#	pragma once

#	include "WindowsIncluder.h"

namespace Menge
{
	class CriticalErrorsMonitor
	{
	public:
		static void run( const char * _versionInfo, const WCHAR * _userPath, const WCHAR * _logFileName );
	};
}
