#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#	include "ThreadQueue.h"

#	include "Factory/FactoryStore.h"

#	include <vector>

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
        bool initialize( size_t _threadCount ) override;
        void finalize() override;

	public:
		bool createThread( const ConstString & _threadName, int _priority ) override;

    public:
        bool addTask( const ConstString & _threadName, const ThreadTaskInterfacePtr & _task ) override;
        void joinTask( const ThreadTaskInterfacePtr & _task ) override;

	public:
		ThreadQueueInterfacePtr runTaskQueue( const ConstString & _threadName, size_t _countThread, size_t _packetSize ) override;

	public:
        void update() override;

    public:
        ThreadMutexInterfacePtr createMutex() override;

    public:
        void sleep( unsigned int _ms ) override;
		uint32_t getCurrentThreadId() override;

    protected:
		bool isTaskOnProgress_( const ThreadTaskInterfacePtr & _task, ThreadIdentityPtr & _identity ) const;
		bool hasThread_( const ConstString & _name ) const;
		
	protected:
        ServiceProviderInterface * m_serviceProvider;

        size_t m_threadCount;
		        
		struct ThreadTaskDesc
		{
			ThreadTaskInterfacePtr task;
			ThreadIdentityPtr identity;
			ConstString threadName;
			bool progress;
			bool complete;
		};

        typedef std::vector<ThreadTaskDesc> TVectorThreadTaskDesc;
        TVectorThreadTaskDesc m_tasks;
				
		typedef std::vector<ThreadQueuePtr> TVectorThreadQueues;
		TVectorThreadQueues m_threadQueues;

		typedef FactoryPoolStore<ThreadQueue, 4> TFactoryThreadQueue;
		TFactoryThreadQueue m_factoryThreadQueue;

		struct ThreadDesc
		{
			ConstString name;
			ThreadIdentityPtr identity;
		};

		typedef std::vector<ThreadDesc> TVectorThreads;
		TVectorThreads m_threads;
	};
}	// namespace Menge
