#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#	include "ThreadPool.h"

#	include "Factory/FactoryPool.h"

#	include <vector>

namespace Menge
{    
    struct ThreadTaskHandle
    {
        ThreadTaskInterfacePtr task;
        ThreadIdentityPtr identity;
    };

	class ThreadEngine
        : public ThreadServiceInterface
	{
	public:
		ThreadEngine();
		~ThreadEngine();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize( size_t _threadCount ) override;
        void finalize() override;

    public:
        size_t taskInProgress() const;
        bool isTaskOnProgress( const ThreadTaskInterfacePtr & _task ) const;

    public:
        bool addTask( const ThreadTaskInterfacePtr & _task, int _priority ) override;
        void joinTask( const ThreadTaskInterfacePtr & _task ) override;
        void cancelTask( const ThreadTaskInterfacePtr & _task ) override;

	public:
		ThreadPoolInterfacePtr runTaskPool() override;

	public:
        void update() override;

    public:
        ThreadMutexInterfacePtr createMutex() override;

    public:
        void sleep( unsigned int _ms ) override;

    protected:
        void testComplete_();
        bool joinTask_( const ThreadTaskInterfacePtr & _task );

    protected:
        bool getThreadIdentity_( const ThreadTaskInterfacePtr & _task, ThreadIdentityPtr & _identity );

	protected:
        ServiceProviderInterface * m_serviceProvider;

        size_t m_threadCount;
        
        typedef std::vector<ThreadTaskHandle> TVectorThreadTaskHandle;
        TVectorThreadTaskHandle m_taskThread;
				
		typedef std::vector<ThreadPoolPtr> TVectorThreadPool;
		TVectorThreadPool m_threadPools;

		typedef FactoryPool<ThreadPool, 4> TFactoryThreadPool;
		TFactoryThreadPool m_factoryThreadPool;
	};
}	// namespace Menge
