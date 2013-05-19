#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#   include "WindowsLayer/WindowsIncluder.h"

namespace Menge
{
	class Win32ThreadIdentity;
	class ServiceProviderInterface;

	class Win32ThreadSystem
		: public ThreadSystemInterface
	{
	public:
		Win32ThreadSystem();
		~Win32ThreadSystem();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        void destroy() override;

	public:
		bool initialize() override;
		void finalize() override;

	public:
		ThreadIdentity * createThread( ThreadListener * _listener ) override;
		void joinThread( ThreadIdentity * _thread ) override;
		void sleep( unsigned int _ms ) override;
	
	protected:
		typedef std::vector<Win32ThreadIdentity *> TVectorPosixThreadIdentity;
		TVectorPosixThreadIdentity m_threadIdentities;

		ServiceProviderInterface * m_serviceProvider;
	};
}
