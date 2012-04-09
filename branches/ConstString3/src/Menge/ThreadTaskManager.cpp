#	include "ThreadTaskManager.h"
#	include "ThreadTask.h"
#	include "ThreadTaskPacket.h"
#	include "LogEngine.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskManager::ThreadTaskManager( ThreadSystemInterface * _interface, size_t _threadCount )
		: m_threadSystem(_interface)
		, m_threadCount(_threadCount)
	{	
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskManager::~ThreadTaskManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskManager::initialize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskManager::finalize()
	{	
		for( TVectorTask::iterator 
			it =  m_tasks.begin(),
			it_end = m_tasks.end();
		it != it_end;
		++it )
		{
			ThreadTask * task = *it;
			task->destroy();
		}
		
		m_tasks.clear();

		for( TVectorThreadTask::iterator 
			it =  m_taskThread.begin(),
			it_end = m_taskThread.end();
		it != it_end;
		++it )
		{
			ThreadTaskHandle & taskThread = *it;

			ThreadTask * task = taskThread.task;
			joinTask_(task);
		}

		m_taskThread.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ThreadTaskManager::taskInProgress() const
	{
		return m_taskThread.size();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskManager::isTaskOnProgress( ThreadTask * _task ) const
	{
		for( TVectorThreadTask::const_iterator 
			it =  m_taskThread.begin(),
			it_end = m_taskThread.end();
		it != it_end;
		++it )
		{
			const ThreadTaskHandle & taskThread = *it;
			const ThreadTask * task = taskThread.task;
			
			if( _task == task )
			{
				return true;
			}
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskManager::addTask( ThreadTask * _task )
	{
		m_tasks.push_back( _task );
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskManager::addTaskPacket( ThreadTaskPacket * _taskPacket )
	{
		const TVectorTask & tasks = _taskPacket->getTasks();

		for( TVectorTask::const_iterator
			it = tasks.begin(),
			it_end = tasks.end();
		it != it_end;
		++it )
		{
			ThreadTask * task = *it;
		
			this->addTask( task );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskManager::joinTask( ThreadTask * _task )
	{
		TVectorTask::iterator it_add_find = std::find( m_tasks.begin(), m_tasks.end(), _task );
		
		if( it_add_find != m_tasks.end() )
		{
			_task->onRun();
			
			ThreadIdentity * threadIdentity = m_threadSystem->createThread( _task );

			if( threadIdentity == 0 )
			{
				return;
			}

			m_threadSystem->joinThread( threadIdentity );
					
			if( _task->isInterrupt() == true )
			{
				_task->onInterrupt();
			}
			else if( _task->isComplete() == true )
			{
				_task->onComplete();
			}
						
			m_tasks.erase( it_add_find );
			return;
		}

		if( isTaskOnProgress( _task ) == false )
		{
			MENGE_LOG_ERROR( "ThreadTaskManager.joinTask: Requested task  not exist " );
			return;
		}

		joinTask_(_task);
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskManager::joinTask_( ThreadTask * _task )
	{
		ThreadIdentity * threadIdentity = this->getThreadIdentity_( _task );

		if( threadIdentity == NULL )
		{
			return;
		}

		m_threadSystem->joinThread( threadIdentity );
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskManager::cancelTask( ThreadTask * _task )
	{
		TVectorTask::iterator it_add_find = std::find( m_tasks.begin(), m_tasks.end(), _task );
		
		if( it_add_find != m_tasks.end() )
		{
			m_tasks.erase( it_add_find );
			return;
		}

		if( isTaskOnProgress( _task ) == false )
		{
			MENGE_LOG_ERROR( "ThreadTaskManager.cancelTask: Requested task  not exist " );
			return;
		}

		_task->cancel();
		joinTask_(_task);
	}
	///////////////////////////////////////////////////////////////////////////
	void ThreadTaskManager::update()
	{
		this->testComplete_();
		this->injectTasks_();
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class FThreadTaskComplete
		{
		public:
			bool operator () ( ThreadTaskHandle & taskThread ) const
			{			
				ThreadTask * task = taskThread.task;

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
	void ThreadTaskManager::testComplete_()
	{
		/*class FTaskThreadUpdate
		{
		public:
		void operator () ( TaskThread & taskThread ) const
		{			
		Task * task = taskThread.task;

		task->update();
		}
		};

		std::for_each( m_taskThread.begin(), m_taskThread.end(), FTaskThreadUpdate() );*/
		
		TVectorThreadTask::iterator it_remove = 
			std::partition( m_taskThread.begin(), m_taskThread.end(), FThreadTaskComplete());

		for( TVectorThreadTask::iterator 
			it = it_remove,
			it_end = m_taskThread.end();
		it != it_end;
		++it )
		{
			ThreadTaskHandle & taskThread = *it;

			ThreadTask * task = taskThread.task;

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
	void ThreadTaskManager::injectTasks_()
	{
		while( m_tasks.empty() == false )
		{
			if( m_taskThread.size() == m_threadCount )
			{
				break;
			}

			ThreadTask * task = m_tasks.back();
			m_tasks.pop_back();
			bool state = task->onRun();
						
			if( state == false )
			{
				continue;
			}

			ThreadIdentity * threadIdentity = m_threadSystem->createThread( task );

			if( threadIdentity == 0 )
			{
				continue;
			}

			ThreadTaskHandle taskThread;

			taskThread.task = task;
			taskThread.identity = threadIdentity;

			m_taskThread.push_back( taskThread );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadIdentity * ThreadTaskManager::getThreadIdentity_( ThreadTask * _task )
	{
		for( TVectorThreadTask::iterator 
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

}