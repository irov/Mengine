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

			ThreadTaskInterface * task = taskThread.task;

			this->joinTask_(task);
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
	bool ThreadEngine::isTaskOnProgress( ThreadTaskInterface * _task ) const
	{
		for( TVectorThreadTaskHandle::const_iterator 
			it = m_taskThread.begin(),
			it_end = m_taskThread.end();
		it != it_end;
		++it )
		{
			const ThreadTaskHandle & taskThread = *it;
			const ThreadTaskInterface * task = taskThread.task;
			
			if( _task == task )
			{
				return true;
			}
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::addTask( ThreadTaskInterface * _task, int _priority )
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

        ThreadIdentity * threadIdentity = THREAD_SYSTEM(m_serviceProvider)
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
	void ThreadEngine::addTaskPacket( ThreadTaskPacket * _taskPacket )
	{
		const TVectorThreadTask & tasks = _taskPacket->getTasks();

		for( TVectorThreadTask::const_iterator
			it = tasks.begin(),
			it_end = tasks.end();
		it != it_end;
		++it )
		{
			ThreadTaskInterface * task = *it;
		
			this->addTask( task, 0 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadEngine::joinTask( ThreadTaskInterface * _task )
	{
		if( this->isTaskOnProgress( _task ) == false )
		{
			return;
		}

		this->joinTask_( _task );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::joinTask_( ThreadTaskInterface * _task )
	{
		ThreadIdentity * threadIdentity = this->getThreadIdentity_( _task );

		if( threadIdentity == nullptr )
		{
            LOGGER_ERROR(m_serviceProvider)("ThreadEngine::joinTask_: threadIdentity is null"
                );

			return false;
		}

		_task->cancel();

		if( THREAD_SYSTEM(m_serviceProvider)
            ->joinThread( threadIdentity ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ThreadEngine::joinTask_: invalid join thread"
                );

            return false;
        }

        this->testComplete_();

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadEngine::cancelTask( ThreadTaskInterface * _task )
	{
		if( this->isTaskOnProgress( _task ) == false )
		{
			return;
		}

		_task->cancel();
	}
	///////////////////////////////////////////////////////////////////////////
	void ThreadEngine::update()
	{
		this->testComplete_();
		//this->injectTasks_();
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadEngine::testComplete_()
	{
		if( m_taskThread.empty() == true )
		{
			return;
		}
		
		for( TVectorThreadTaskHandle::iterator 
			it = m_taskThread.begin(),
			it_end = m_taskThread.end();
		it != it_end;
		/*++it*/ )
		{
			ThreadTaskInterface * task = it->task;
			
			if( task->update() == false )
			{
				++it;
			}
			else
			{
				it = m_taskThread.erase( it );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadIdentity * ThreadEngine::getThreadIdentity_( ThreadTaskInterface * _task )
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

			ThreadIdentity * threadIdentity = taskThread.identity;

			return threadIdentity;
		}

		return nullptr;
	}
    //////////////////////////////////////////////////////////////////////////
    ThreadMutexInterface * ThreadEngine::createMutex()
    {
        ThreadMutexInterface * mutex = THREAD_SYSTEM(m_serviceProvider)
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
