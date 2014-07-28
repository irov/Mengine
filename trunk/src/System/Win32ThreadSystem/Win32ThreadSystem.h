#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#   include "WindowsLayer/WindowsIncluder.h"

#	include "Win32ThreadIdentity.h"
#	include "Win32ThreadMutex.h"

#   include "Factory/FactoryStore.h"

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
		ThreadIdentityPtr createThread( int _priority ) override;

	public:
        ThreadMutexInterfacePtr createMutex() override;

	public:
		void sleep( unsigned int _ms ) override;

	public:
		uint32_t getCurrentThreadId() const override;

	protected:
		void onThreadIdentityRemove_( Win32ThreadIdentity * _identity );
	
	protected:
        typedef FactoryPoolStore<Win32ThreadIdentity, 16> TPoolWin32ThreadIdentity;
        TPoolWin32ThreadIdentity m_poolWin32ThreadIdentity;

        typedef FactoryPoolStore<Win32ThreadMutex, 16> TPoolWin32ThreadMutex;
        TPoolWin32ThreadMutex m_poolWin32ThreadMutex;

		//typedef std::vector<Win32ThreadIdentityPtr> TVectorPosixThreadIdentity;
		//TVectorPosixThreadIdentity m_threadIdentities;

		ServiceProviderInterface * m_serviceProvider;
	};
}
