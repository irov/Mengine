#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#	include "MarmaladeThreadIdentity.h"
#   include "MarmaladeThreadMutex.h"

#   include "s3eThread.h"

#   include "Factory/FactoryStore.h"

#   include "stdex/pool.h"

#	include <vector>

namespace Menge
{
	class MarmaladeThreadIdentity;
	class ServiceProviderInterface;
	class LogServiceInterface;

	class MarmaladeThreadSystem
		: public ThreadSystemInterface
	{
	public:
		MarmaladeThreadSystem();
		~MarmaladeThreadSystem();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
		void finalize() override;

	public:
		ThreadIdentityPtr createThread( int _priority ) override;
		
	public:
		void sleep( unsigned int _ms ) override;

    public:
        ThreadMutexInterfacePtr createMutex() override;

	public:
		uint32_t getCurrentThreadId() const override;
	
	protected:
        typedef FactoryPoolStore<MarmaladeThreadIdentity, 16> TPoolMarmaladeThreadIdentity;
        TPoolMarmaladeThreadIdentity m_poolWin32ThreadIdentity;

        typedef FactoryPoolStore<MarmaladeThreadMutex, 16> TPoolMarmaladeThreadMutex;
        TPoolMarmaladeThreadMutex m_poolMarmaladeThreadMutex;

		typedef std::vector<MarmaladeThreadIdentityPtr> TVectorPosixThreadIdentity;
		TVectorPosixThreadIdentity m_threadIdentities;

		ServiceProviderInterface * m_serviceProvider;
	};
}
