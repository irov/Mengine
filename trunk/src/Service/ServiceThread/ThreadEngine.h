#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#	include "ThreadQueue.h"
#	include "ThreadMutexDummy.h"

#	include "Factory/FactoryStore.h"

#	include <stdex/stl_vector.h>

namespace Menge
{    
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
        bool initialize( uint32_t _threadCount ) override;
        void finalize() override;

	public:
		bool createThread( const ConstString & _threadName, int _priority ) override;
		bool destroyThread( const ConstString & _threadName ) override;

    public:
        bool addTask( const ConstString & _threadName, const ThreadTaskInterfacePtr & _task ) override;
        bool joinTask( const ThreadTaskInterfacePtr & _task ) override;

	public:
		ThreadQueueInterfacePtr runTaskQueue( const ConstString & _threadName, uint32_t _countThread, uint32_t _packetSize ) override;

	public:
        void update() override;

    public:
        ThreadMutexInterfacePtr createMutex() override;

    public:
        void sleep( uint32_t _ms ) override;
		ptrdiff_t getCurrentThreadId() override;

    protected:
		bool isTaskOnProgress_( const ThreadTaskInterfacePtr & _task, ThreadIdentityPtr & _identity ) const;
		bool hasThread_( const ConstString & _name ) const;
		
	protected:
        ServiceProviderInterface * m_serviceProvider;

		ThreadMutexInterfacePtr m_allocatorPoolMutex;

        uint32_t m_threadCount;
		        
		struct ThreadTaskDesc
		{
			ThreadTaskInterfacePtr task;
			ThreadIdentityPtr identity;
			ConstString threadName;
			bool progress;
			bool complete;
		};

        typedef stdex::vector<ThreadTaskDesc> TVectorThreadTaskDesc;
        TVectorThreadTaskDesc m_tasks;
				
		typedef stdex::vector<ThreadQueuePtr> TVectorThreadQueues;
		TVectorThreadQueues m_threadQueues;

		typedef FactoryPoolStore<ThreadQueue, 4> TFactoryThreadQueue;
		TFactoryThreadQueue m_factoryThreadQueue;

		typedef FactoryPoolStore<ThreadMutexDummy, 16> TPoolThreadMutexDummy;
		TPoolThreadMutexDummy m_poolThreadMutexDummy;	

		struct ThreadDesc
		{
			ConstString name;
			ThreadIdentityPtr identity;
		};

		typedef stdex::vector<ThreadDesc> TVectorThreads;
		TVectorThreads m_threads;

		bool m_threadAvaliable;
	};
}	// namespace Menge
