#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#   include "WindowsLayer/WindowsIncluder.h"

#	include "Win32ThreadIdentity.h"
#	include "Win32ThreadMutex.h"

#   include "Factory/FactoryPool.h"

#   include "stdex/pool.h"

#	include <vector>

namespace Menge
{
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
		bool initialize() override;
		void finalize() override;

	public:
		ThreadIdentity * createThread( ThreadTaskInterface * _listener, int _priority ) override;
		bool joinThread( ThreadIdentity * _thread ) override;

        ThreadMutexInterface * createMutex() override;

		void sleep( unsigned int _ms ) override;
	
	protected:
        typedef stdex::template_pool<Win32ThreadIdentity, 16> TPoolWin32ThreadIdentity;
        TPoolWin32ThreadIdentity m_poolWin32ThreadIdentity;

        typedef FactoryPool<Win32ThreadMutex, 16> TPoolWin32ThreadMutex;
        TPoolWin32ThreadMutex m_poolWin32ThreadMutex;

		typedef std::vector<Win32ThreadIdentity *> TVectorPosixThreadIdentity;
		TVectorPosixThreadIdentity m_threadIdentities;

		ServiceProviderInterface * m_serviceProvider;
	};
}
