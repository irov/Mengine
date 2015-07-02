#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Config/Typedef.h"
#	include "Config/String.h"

#	include "WindowsLayer/WindowsIncluder.h"

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
		AlreadyRunningMonitor( ServiceProviderInterface * _serviceProvider );
		~AlreadyRunningMonitor();

	public:
		bool run( int _policy, const WString & _windowClassName, const WString & _title );
		void stop();

	protected:
		ServiceProviderInterface * m_serviceProvider;

		HANDLE m_mutex;		
	};
}
