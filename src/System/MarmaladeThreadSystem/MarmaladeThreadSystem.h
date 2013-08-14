#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#	include "MarmaladeThreadIdentity.h"
#   include "MarmaladeThreadMutex.h"

#   include "s3eThread.h"

#   include "Factory/FactoryPool.h"

#   include "stdex/pool.h"

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
		ThreadIdentity * createThread( ThreadListener * _listener, int _priority ) override;
		bool joinThread( ThreadIdentity * _thread ) override;
		void sleep( unsigned int _ms ) override;

    public:
        ThreadMutexInterface * createMutex() override;
	
	protected:
        typedef stdex::template_pool<MarmaladeThreadIdentity, 16> TPoolMarmaladeThreadIdentity;
        TPoolMarmaladeThreadIdentity m_poolWin32ThreadIdentity;

        typedef FactoryPool<MarmaladeThreadMutex, 16> TPoolMarmaladeThreadMutex;
        TPoolMarmaladeThreadMutex m_poolMarmaladeThreadMutex;

		typedef std::vector<MarmaladeThreadIdentity *> TVectorPosixThreadIdentity;
		TVectorPosixThreadIdentity m_threadIdentities;

		ServiceProviderInterface * m_serviceProvider;
	};
}
