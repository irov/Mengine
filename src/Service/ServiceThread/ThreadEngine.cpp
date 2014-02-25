#	include "ThreadEngine.h"

#	include "Kernel/ThreadTask.h"
#	include "Kernel/ThreadTaskPacket.h"

#	include "Logger/Logger.h"

#	include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ThreadService, Menge::ThreadServiceInterface, Menge::ThreadEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadEngine::ThreadEngine()
		: m_serviceProvider(nullptr)      
        , m_threadCount(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadEngine::~ThreadEngine()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void ThreadEngine::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {        
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * ThreadEngine::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::initialize( size_t _threadCount )
	{
        m_threadCount = _threadCount;
                
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadEngine::finalize()
	{	
		for( TVectorThreadTaskHandle::iterator 
			it =  m_taskThread.begin(),
			it_end = m_taskThread.end();
		it != it_end;
		++it )
		{
			ThreadTaskHandle & taskThread = *it;

			const ThreadTaskInterfacePtr & task = taskThread.task;

			this->joinTask_( task );
		}

		m_taskThread.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ThreadEngine::taskInProgress() const
	{
        size_t count = m_taskThread.size();

		return count;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::isTaskOnProgress( const ThreadTaskInterfacePtr & _task ) const
	{
		for( TVectorThreadTaskHandle::const_iterator 
			it = m_taskThread.begin(),
			it_end = m_taskThread.end();
		it != it_end;
		++it )
		{
			const ThreadTaskHandle & taskThread = *it;
			const ThreadTaskInterfacePtr & task = taskThread.task;
			
			if( _task == task )
			{
				return true;
			}
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::addTask( const ThreadTaskInterfacePtr & _task, int _priority )
	{
        if( m_taskThread.size() == m_threadCount )
        {
            LOGGER_ERROR(m_serviceProvider)("ThreadEngine::addTask is maximum %d"
                , m_threadCount
                );

            return false;
        }

        bool state = _task->run();

        if( state == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ThreadEngine::addTask invalid run"               
                );

            return false;
        }

        ThreadIdentityPtr threadIdentity = THREAD_SYSTEM(m_serviceProvider)
            ->createThread( _task, _priority );

        if( threadIdentity == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("ThreadEngine::addTask invalid create identity"               
                );

            return false;
        }

        ThreadTaskHandle taskThread;

        taskThread.task = _task;
        taskThread.identity = threadIdentity;

        m_taskThread.push_back( taskThread );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadEngine::joinTask( const ThreadTaskInterfacePtr & _task )
	{
		if( this->isTaskOnProgress( _task ) == false )
		{
			return;
		}

		this->joinTask_( _task );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::joinTask_( const ThreadTaskInterfacePtr & _task )
	{
		ThreadIdentityPtr threadIdentity;
		if( this->getThreadIdentity_( _task, threadIdentity ) == false )
		{
            LOGGER_ERROR(m_serviceProvider)("ThreadEngine::joinTask_: threadIdentity is null"
                );

			return false;
		}

		if( _task->cancel() == false )
		{
			return true;
		}

		if( THREAD_SYSTEM(m_serviceProvider)
            ->joinThread( threadIdentity ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ThreadEngine::joinTask_: invalid join thread"
                );

            return false;
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadEngine::cancelTask( const ThreadTaskInterfacePtr & _task )
	{
		if( this->isTaskOnProgress( _task ) == false )
		{
			return;
		}

		_task->cancel();
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadPoolInterfacePtr ThreadEngine::runTaskPool()
	{
		ThreadPoolPtr taskPool = m_factoryThreadPool.createObjectT();

		taskPool->setServiceProvider( m_serviceProvider );

		m_threadPools.push_back( taskPool );

		return taskPool;
	}
	///////////////////////////////////////////////////////////////////////////
	void ThreadEngine::update()
	{
		this->testComplete_();
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadEngine::testComplete_()
	{
		for( TVectorThreadTaskHandle::size_type it = 0,
			it_end = m_taskThread.size();
		it != it_end;
		/*++it*/ )
		{
			const ThreadTaskHandle & handle = m_taskThread[it];

			const ThreadTaskInterfacePtr & task = handle.task;

			if( task->update() == false )
			{
				++it;
			}
			else
			{
				m_taskThread[it] = m_taskThread.back();
				m_taskThread.pop_back();
				--it_end;
			}
		}

		for( TVectorThreadPool::size_type 
			it = 0,
			it_end = m_threadPools.size();
		it != it_end;
		/*++it*/ )
		{
			const ThreadPoolPtr & pool = m_threadPools[it];

			if( pool->update() == false )
			{
				++it;
			}
			else
			{
				m_threadPools[it] = m_threadPools.back();
				m_taskThread.pop_back();
				--it_end;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::getThreadIdentity_( const ThreadTaskInterfacePtr & _task, ThreadIdentityPtr & _identity )
	{
		for( TVectorThreadTaskHandle::iterator 
			it = m_taskThread.begin(),
			it_end = m_taskThread.end();
		it != it_end;
		++it )
		{
			ThreadTaskHandle & taskThread = *it;

			if( taskThread.task != _task )
			{
				continue;
			}

			_identity = taskThread.identity;

			return true;
		}

		return false;
	}
    //////////////////////////////////////////////////////////////////////////
    ThreadMutexInterfacePtr ThreadEngine::createMutex()
    {
        ThreadMutexInterfacePtr mutex = THREAD_SYSTEM(m_serviceProvider)
            ->createMutex();

        return mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadEngine::sleep( unsigned int _ms )
    {
        THREAD_SYSTEM(m_serviceProvider)
            ->sleep( _ms );
    }
}	// namespace Menge
