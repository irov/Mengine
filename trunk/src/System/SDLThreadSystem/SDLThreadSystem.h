#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#	include "SDLThreadIdentity.h"
#   include "SDLThreadMutex.h"

#   include "Factory/FactoryStore.h"

#   include "stdex/pool.h"

namespace Menge
{
	class SDLThreadSystem
		: public ThreadSystemInterface
	{
	public:
		SDLThreadSystem();
		~SDLThreadSystem();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
		void finalize() override;

	public:
		bool avaliable() const override;

	public:
		ThreadIdentityPtr createThread( int _priority ) override;
		
	public:
		void sleep( unsigned int _ms ) override;

    public:
        ThreadMutexInterfacePtr createMutex() override;

	public:
		ptrdiff_t getCurrentThreadId() const override;
	
	protected:
		ServiceProviderInterface * m_serviceProvider;

        typedef FactoryPoolStore<SDLThreadIdentity, 16> TPoolThreadIdentity;
		TPoolThreadIdentity m_poolThreadIdentity;

		typedef FactoryPoolStore<SDLThreadMutex, 16> TPoolThreadMutex;
		TPoolThreadMutex m_poolThreadMutex;
	};
}
