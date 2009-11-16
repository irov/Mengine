#	pragma once

#	include "Config/Typedef.h"
#	include "WindowsIncluder.h"

namespace Menge
{
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
		AlreadyRunningMonitor();

	public:
		bool run( int _policy, const String & _title );
		void stop();

	protected:
		HANDLE m_mutex;		
	};
}