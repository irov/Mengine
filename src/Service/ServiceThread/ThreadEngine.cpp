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
		: m_serviceProvider(NULL)      
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
	bool ThreadEngine::addTask( ThreadTaskInterface * _task )
	{
        if( m_taskThread.size() == m_threadCount )
        {
            LOGGER_ERROR(m_serviceProvider)("ThreadEngine::addTask is maximum %d"
                , m_threadCount
                );

            return false;
        }

        bool state = _task->onRun();

        if( state == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ThreadEngine::addTask invalid run"               
                );

            return false;
        }

        ThreadIdentity * threadIdentity = THREAD_SYSTEM(m_serviceProvider)->createThread( _task );

        if( threadIdentity == 0 )
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
		
			this->addTask( task );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::joinTask( ThreadTaskInterface * _task )
	{
		if( this->isTaskOnProgress( _task ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadEngine.joinTask: Requested task not exist"
                );

			return false;
		}

		this->joinTask_( _task );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::joinTask_( ThreadTaskInterface * _task )
	{
		ThreadIdentity * threadIdentity = this->getThreadIdentity_( _task );

		if( threadIdentity == NULL )
		{
			return false;
		}

		THREAD_SYSTEM(m_serviceProvider)
            ->joinThread( threadIdentity );

        this->testComplete_();

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::cancelTask( ThreadTaskInterface * _task )
	{
		if( this->isTaskOnProgress( _task ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadEngine.cancelTask: Requested task  not exist"
                );

			return false;
		}

		_task->cancel();

		this->joinTask_( _task );

        return true;
	}
	///////////////////////////////////////////////////////////////////////////
	void ThreadEngine::update()
	{
		this->testComplete_();
		//this->injectTasks_();
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class FThreadTaskComplete
		{
		public:
			bool operator () ( ThreadTaskHandle & taskThread ) const
			{			
				ThreadTaskInterface * task = taskThread.task;

				task->update();

				if( task->isComplete() == true )
				{
					return false;
				}

				if( task->isInterrupt() == true )
				{
					return false;
				}

				return true;
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadEngine::testComplete_()
	{
		if( m_taskThread.empty() == true )
		{
			return;
		}
		
		TVectorThreadTaskHandle::iterator it_remove = 
			std::partition( m_taskThread.begin(), m_taskThread.end(), FThreadTaskComplete());

		for( TVectorThreadTaskHandle::iterator 
			it = it_remove,
			it_end = m_taskThread.end();
		it != it_end;
		++it )
		{
			ThreadTaskHandle & taskThread = *it;

			ThreadTaskInterface * task = taskThread.task;

			if( task->isInterrupt() == true )
			{
				task->onInterrupt();
			}
			else if( task->isComplete() == true )
			{
				task->onComplete();
			}
		}

		m_taskThread.erase( it_remove, m_taskThread.end() );
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

		return NULL;
	}
    //////////////////////////////////////////////////////////////////////////
    void ThreadEngine::sleep( unsigned int _ms )
    {
        THREAD_SYSTEM(m_serviceProvider)
            ->sleep( _ms );
    }
}	// namespace Menge
