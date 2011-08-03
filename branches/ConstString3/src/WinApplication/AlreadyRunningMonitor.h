#	pragma once

#	include "Config/Typedef.h"
#	include "WindowsIncluder.h"

namespace Menge
{
	class LogSystemInterface;

	// already running policy
	enum EAlreadyRunningPolicy
	{
		EARP_NONE,
		EARP_SETFOCUS,
		EARP_SHOWMESSAGE
	};

	class AlreadyRunningMonitor
	{
	public:
		AlreadyRunningMonitor( LogSystemInterface * _logSystem );

	public:
		bool run( int _policy, const String & _title );
		void stop();

	protected:
		LogSystemInterface * m_logSystem;

		HANDLE m_mutex;		
	};
}
