#	pragma once

#	include "Config/Typedef.h"

#	include "WindowsLayer/WindowsIncluder.h"

namespace Menge
{
	class ServiceProviderInterface;

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

	public:
		bool run( int _policy, const WString & _windowClassName, const WString & _title );
		void stop();

	protected:
		ServiceProviderInterface * m_serviceProvider;

		HANDLE m_mutex;		
	};
}
