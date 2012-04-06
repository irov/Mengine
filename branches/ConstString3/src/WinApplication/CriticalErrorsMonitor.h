#	pragma once

namespace Menge
{
	class CriticalErrorsMonitor
	{
	public:
		static void run( const char * _versionInfo, const wchar_t * _userPath, const wchar_t * _logFileName );
	};
}
