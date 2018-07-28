#include "ThreadEngine.h"

#include "Interface/ConfigInterface.h"

#include "Kernel/ThreadTask.h"
#include "Kernel/ThreadTaskPacket.h"

#include "Kernel/FactoryPool.h"

#include "Kernel/Logger.h"

#include "stdex/allocator.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ThreadService, Mengine::ThreadEngine );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	ThreadEngine::ThreadEngine()
		: m_threadCount(0)
		, m_avaliable(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadEngine::~ThreadEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_stdex_thread_lock( ThreadMutexInterface * _mutex )
	{
		_mutex->lock();
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_stdex_thread_unlock( ThreadMutexInterface * _mutex )
	{
		_mutex->unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::_initializeService()
	{
        m_factoryThreadMutexDummy = new FactoryPool<ThreadMutexDummy, 16>();
        m_factoryThreadQueue = new FactoryPool<ThreadQueue, 4>();
        m_factoryThreadJob = new FactoryPool<ThreadJob, 16>();
        
		bool avaliable = CONFIG_VALUE("Engine", "ThreadServiceAvaliable", true );

		if( avaliable == false )
		{
            m_avaliable = false;
            
			return true;
		}

		if( SERVICE_EXIST( Mengine::ThreadSystemInterface ) == false )
		{
			m_avaliable = false;

			return true;
		}

		m_avaliable = THREAD_SYSTEM()
			->avaliable();

		if( m_avaliable == false )
		{
			return true;
		}

		m_threadCount = CONFIG_VALUE( "Engine", "ThreadCount", 16U );

		m_mutexAllocatorPool = THREAD_SYSTEM()
			->createMutex( __FILE__, __LINE__ );

		stdex_allocator_initialize_threadsafe( m_mutexAllocatorPool.get()
			, (stdex_allocator_thread_lock_t)&s_stdex_thread_lock
			, (stdex_allocator_thread_unlock_t)&s_stdex_thread_unlock 
			);
                        
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadEngine::_finalizeService()
	{	
        for( const ThreadTaskDesc & desc : m_tasks )
		{
            const ThreadTaskInterfacePtr & task = desc.task;

            task->cancel();

            if( desc.progress == false )
            {
                continue;
            }

            const ThreadIdentityInterfacePtr & threadIdentity = desc.identity;

            if( threadIdentity->completeTask() == false )
            {
                threadIdentity->join();
            }
		}

		m_tasks.clear();

        m_threadQueues.clear();

        for( ThreadDesc & desc : m_threads )
		{
			desc.identity->join();
		}

		m_threads.clear();

		stdex_allocator_finalize_threadsafe();

		m_mutexAllocatorPool = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadQueue );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadMutexDummy );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadJob );

        m_factoryThreadQueue = nullptr;
        m_factoryThreadMutexDummy = nullptr;
        m_factoryThreadJob = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::avaliable() const
	{ 
		return m_avaliable;
	}
    //////////////////////////////////////////////////////////////////////////
    ThreadJobPtr ThreadEngine::createJob( uint32_t _sleep )
    {
        ThreadJobPtr threadJob = m_factoryThreadJob->createObject();
        
        if( threadJob->initialize( _sleep ) == false )
        {
            LOGGER_ERROR("ThreadEngine::createJob invalid create"
                );

            return nullptr;
        }

        return threadJob;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::createThread( const ConstString & _threadName, int _priority, const char * _file, uint32_t _line )
	{
		if( m_avaliable == false )
		{
			return false;
		}

		if( this->hasThread( _threadName ) == true )
		{
			return false;
		}

		ThreadIdentityInterfacePtr identity = THREAD_SYSTEM()
			->createThread( _priority, _file, _line );

		if( identity == nullptr )
		{
			return false;
		}

		ThreadDesc td;
		td.name = _threadName;
		td.identity = identity;

		m_threads.emplace_back( td );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::destroyThread( const ConstString & _threadName )
	{
		for( VectorThreadDescs::iterator
			it = m_threads.begin(),
			it_end = m_threads.end();
		it != it_end;
		++it )
		{
			ThreadDesc & td = *it;

			if( td.name != _threadName )
			{
				continue;
			}

			td.identity->completeTask();
			td.identity->join();

            m_threads.erase( it );

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::hasThread( const ConstString & _name ) const
	{
        for( const ThreadDesc & td : m_threads )
		{
			if( td.name == _name )
			{
				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::addTask( const ConstString & _threadName, const ThreadTaskInterfacePtr & _task )
	{
		if( m_avaliable == false )
		{
			return false;
		}

		if( this->hasThread( _threadName ) == false )
		{
			return false;
		}
		
        bool state = _task->run();

        if( state == false )
        {
            LOGGER_ERROR("ThreadEngine::addTask invalid run"               
                );

            return false;
        }

        ThreadTaskDesc desc;

        desc.task = _task;
        desc.threadName = _threadName;
		desc.identity = nullptr;
		desc.progress = false;
		desc.complete = false;

        m_tasks.emplace_back( desc );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::joinTask( const ThreadTaskInterfacePtr & _task )
	{
		_task->cancel();

        for( VectorThreadTaskDesc::const_iterator
            it = m_tasks.begin(),
            it_end = m_tasks.end();
            it != it_end;
            ++it )
        {
            const ThreadTaskDesc & desc = *it;

            const ThreadTaskInterfacePtr & task = desc.task;

            if( _task != task )
            {
                continue;
            }

            if( desc.progress == false )
            {
                m_tasks.erase( it );

                return true;
            }

            const ThreadIdentityInterfacePtr & threadIdentity = desc.identity;

            bool successful = threadIdentity->completeTask();

            if( successful == false )
            {
                threadIdentity->join();
                m_tasks.erase( it );
            }            

            return successful;
        }

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void ThreadEngine::stopTasks()
    {
        for( const ThreadTaskDesc & desc : m_tasks )
        {
            const ThreadTaskInterfacePtr & task = desc.task;

            task->cancel();

            if( desc.progress == false )
            {
                continue;
            }

            const ThreadIdentityInterfacePtr & threadIdentity = desc.identity;

            if( threadIdentity->completeTask() == false )
            {
                threadIdentity->join();
            }
        }
    }
	//////////////////////////////////////////////////////////////////////////
	ThreadQueueInterfacePtr ThreadEngine::runTaskQueue( const ConstString & _threadName, uint32_t _countThread, uint32_t _packetSize )
	{
		if( m_avaliable == false )
		{
			return nullptr;
		}

		ThreadQueuePtr taskQueue = m_factoryThreadQueue->createObject();

		taskQueue->setThreadName( _threadName );
		taskQueue->setThreadCount( _countThread );
		taskQueue->setPacketSize( _packetSize );

		m_threadQueues.emplace_back( taskQueue );

		return taskQueue;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadEngine::cancelTaskQueue( const ThreadQueueInterfacePtr & _queue )
	{
		_queue->cancel();

		for( VectorThreadQueues::iterator
			it = m_threadQueues.begin(),
			it_end = m_threadQueues.end();
			it != it_end;
			++it)
		{
			ThreadQueueInterfacePtr queue = *it;

			if( queue != _queue )
			{
				continue;
			}

			*it = m_threadQueues.back();
			m_threadQueues.pop_back();

			return;
		}
	}
	///////////////////////////////////////////////////////////////////////////
	void ThreadEngine::update()
	{
        for( ThreadTaskDesc & desc_task : m_tasks )
		{
			if( desc_task.complete == true )
			{
				continue;
			}

			if( desc_task.progress == true )
			{
				if( desc_task.task->isComplete() == true )
				{
					desc_task.complete = true;
					desc_task.progress = false;
				}
			}
			else
			{
				ThreadTaskInterface * task = desc_task.task.get();

                for( ThreadDesc & desc_thread : m_threads )
				{
					if( desc_thread.name != desc_task.threadName )
					{
						continue;
					}

					if( desc_thread.identity->processTask( task ) == true )
					{
						desc_task.identity = desc_thread.identity;
						desc_task.progress = true;
						break;
					}
				}
			}
		}

		for( VectorThreadTaskDesc::size_type 
			it_task = 0,
			it_task_end = m_tasks.size();
		it_task != it_task_end;
		/*++it*/ )
		{
			const ThreadTaskDesc & handle = m_tasks[it_task];

			const ThreadTaskInterfacePtr & task = handle.task;

			if( task->update() == false )
			{
				++it_task;
			}
			else
			{
				m_tasks[it_task] = m_tasks.back();
				m_tasks.pop_back();
				--it_task_end;
			}
		}

		for( VectorThreadQueues::size_type 
			it_task = 0,
			it_task_end = m_threadQueues.size();
		it_task != it_task_end;
		/*++it*/ )
		{
			const ThreadQueuePtr & pool = m_threadQueues[it_task];

			if( pool->update() == false )
			{
				++it_task;
			}
			else
			{
				m_threadQueues[it_task] = m_threadQueues.back();
				m_tasks.pop_back();
				--it_task_end;
			}
		}
	}
    //////////////////////////////////////////////////////////////////////////
	ThreadMutexInterfacePtr ThreadEngine::createMutex( const char * _file, uint32_t _line )
    {
		if( m_avaliable == false )
		{
			 ThreadMutexInterfacePtr mutex_dummy = 
				 m_factoryThreadMutexDummy->createObject();

			return mutex_dummy;
		}

        ThreadMutexInterfacePtr mutex = THREAD_SYSTEM()
            ->createMutex( _file, _line );

        return mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadEngine::sleep( uint32_t _ms )
    {
        THREAD_SYSTEM()
            ->sleep( _ms );
    }
	//////////////////////////////////////////////////////////////////////////
	ptrdiff_t ThreadEngine::getCurrentThreadId()
	{
		if( m_avaliable == false )
		{
			return 0U;
		}

		ptrdiff_t id = THREAD_SYSTEM()
			->getCurrentThreadId();

		return id;
	}
}	
