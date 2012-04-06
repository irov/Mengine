#	pragma once

#	include "Config/Typedef.h"
#	include "WindowsIncluder.h"

namespace Menge
{
	class LogServiceInterface;

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
		AlreadyRunningMonitor( LogServiceInterface * _logService );

	public:
		bool run( int _policy, const WString & _windowClassName, const WString & _title );
		void stop();

	protected:
		LogServiceInterface * m_logService;

		HANDLE m_mutex;		
	};
}
