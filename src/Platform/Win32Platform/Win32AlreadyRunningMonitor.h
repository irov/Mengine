#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Config/Typedef.h"
#	include "Config/String.h"

#	include "WIN32/WindowsIncluder.h"

namespace Menge
{	
	// already running policy
	enum EAlreadyRunningPolicy
	{
		EARP_NONE,
		EARP_SETFOCUS,
		EARP_SHOWMESSAGE
	};

	class Win32AlreadyRunningMonitor
	{
	public:
		Win32AlreadyRunningMonitor();
		~Win32AlreadyRunningMonitor();

	public:
		bool run( int _policy, const WChar * _windowClassName, const WString & _title );
		void stop();

	protected:
		HANDLE m_mutex;		
	};
}
