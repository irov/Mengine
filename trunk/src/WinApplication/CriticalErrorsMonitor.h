#	pragma once

namespace Menge
{
	class CriticalErrorsMonitor
	{
	public:
		static void run( const char * _versionInfo, const char* _userPath, const char* _logFileName );
	};
}
