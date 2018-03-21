#pragma once

#include "Interface/ThreadSystemInterface.h"

#include "Core/ServiceBase.h"

#include "ThreadQueue.h"
#include "ThreadMutexDummy.h"

#include "Factory/Factory.h"

#include "stdex/stl_vector.h"

namespace Mengine
{
	class ThreadEngine
        : public ServiceBase<ThreadServiceInterface>
	{
	public:
		ThreadEngine();
		~ThreadEngine();

    public:
        bool _initialize() override;
        void _finalize() override;

	public:
		bool avaliable() const override;

    public:
        ThreadJobPtr createJob( uint32_t _sleep ) override;

	public:
		bool createThread( const ConstString & _threadName, int _priority, const char * _file, uint32_t _line ) override;
		bool destroyThread( const ConstString & _threadName ) override;

    public:
        bool hasThread( const ConstString & _name ) const override;

    public:
        bool addTask( const ConstString & _threadName, const ThreadTaskInterfacePtr & _task ) override;
        bool joinTask( const ThreadTaskInterfacePtr & _task ) override;

    public:
        void stopTasks() override;

	public:
		ThreadQueueInterfacePtr runTaskQueue( const ConstString & _threadName, uint32_t _countThread, uint32_t _packetSize ) override;
		void cancelTaskQueue( const ThreadQueueInterfacePtr & _queue ) override;

	public:
        void update() override;

    public:
		ThreadMutexInterfacePtr createMutex( const char * _file, uint32_t _line ) override;

    public:
        void sleep( uint32_t _ms ) override;
		ptrdiff_t getCurrentThreadId() override;

	protected:
		ThreadMutexInterfacePtr m_mutexAllocatorPool;

        uint32_t m_threadCount;
		        
		struct ThreadTaskDesc
		{
			ThreadTaskInterfacePtr task;
			ThreadIdentityInterfacePtr identity;
			ConstString threadName;
			bool progress;
			bool complete;
		};

        typedef stdex::vector<ThreadTaskDesc> TVectorThreadTaskDesc;
        TVectorThreadTaskDesc m_tasks;
				
		typedef stdex::vector<ThreadQueuePtr> TVectorThreadQueues;
		TVectorThreadQueues m_threadQueues;

		FactoryPtr m_factoryThreadQueue;
        FactoryPtr m_factoryThreadJob;
		FactoryPtr m_factoryThreadMutexDummy;

		struct ThreadDesc
		{
			ConstString name;
			ThreadIdentityInterfacePtr identity;
		};

		typedef stdex::vector<ThreadDesc> TVectorThreads;
		TVectorThreads m_threads;

		bool m_avaliable;
	};
}	
