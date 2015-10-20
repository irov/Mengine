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
		bool avaliable() const override;

	public:
		ThreadIdentityPtr createThread( int _priority, const char * _doc ) override;
		
	public:
		void sleep( unsigned int _ms ) override;

    public:
		ThreadMutexInterfacePtr createMutex( const char * _doc ) override;

	public:
		ptrdiff_t getCurrentThreadId() const override;
	
	protected:
		ServiceProviderInterface * m_serviceProvider;

        typedef FactoryPoolStore<MarmaladeThreadIdentity, 16> TPoolMarmaladeThreadIdentity;
        TPoolMarmaladeThreadIdentity m_poolWin32ThreadIdentity;

        typedef FactoryPoolStore<MarmaladeThreadMutex, 16> TPoolMarmaladeThreadMutex;
        TPoolMarmaladeThreadMutex m_poolMarmaladeThreadMutex;		
	};
}
