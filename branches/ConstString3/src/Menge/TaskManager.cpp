#	include "TaskManager.h"
#	include "Task.h"
#	include "TaskPacket.h"
#	include "LogEngine.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TaskManager::TaskManager( ThreadSystemInterface * _interface, size_t _threadCount )
		: m_threadSystem(_interface)
		, m_threadCount(_threadCount)
	{	
	}
	//////////////////////////////////////////////////////////////////////////
	TaskManager::~TaskManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskManager::initialize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskManager::finalize()
	{	
		for( TVectorTask::iterator 
			it =  m_tasks.begin(),
			it_end = m_tasks.end();
		it != it_end;
		++it )
		{
			Task * task = *it;
			task->destroy();
		}
		
		m_tasks.clear();

		for( TVectorTaskThread::iterator 
			it =  m_taskThread.begin(),
			it_end = m_taskThread.end();
		it != it_end;
		++it )
		{
			TaskThread & taskThread = *it;

			Task * task = taskThread.task;
			joinTask_(task);
		}

		m_taskThread.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	size_t TaskManager::taskInProgress() const
	{
		return m_taskThread.size();
	}
	//////////////////////////////////////////////////////////////////////////
	bool TaskManager::isTaskOnProgress( Task * _task ) const
	{
		for( TVectorTaskThread::const_iterator 
			it =  m_taskThread.begin(),
			it_end = m_taskThread.end();
		it != it_end;
		++it )
		{
			const TaskThread & taskThread = *it;
			const Task * task = taskThread.task;
			
			if( _task == task )
			{
				return true;
			}
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskManager::addTask( Task * _task )
	{
		m_tasks.push_back( _task );
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskManager::addTaskPacket( TaskPacket * _taskPacket )
	{
		const TVectorTask & tasks = _taskPacket->getTasks();

		for( TVectorTask::const_iterator
			it = tasks.begin(),
			it_end = tasks.end();
		it != it_end;
		++it )
		{
			Task * task = *it;
		
			this->addTask( task );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskManager::joinTask( Task * _task )
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
			MENGE_LOG_ERROR( "TaskManager: Requested task  not exist " );
			return;
		}

		joinTask_(_task);
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskManager::joinTask_( Task * _task )
	{
		ThreadIdentity * threadIdentity = this->getThreadIdentity_( _task );

		if( threadIdentity == NULL )
		{
			return;
		}

		m_threadSystem->joinThread( threadIdentity );
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskManager::cancelTask( Task * _task )
	{
		TVectorTask::iterator it_add_find = std::find( m_tasks.begin(), m_tasks.end(), _task );
		
		if( it_add_find != m_tasks.end() )
		{
			m_tasks.erase( it_add_find );
			return;
		}

		if( isTaskOnProgress( _task ) == false )
		{
			MENGE_LOG_ERROR( "TaskManager: Requested task  not exist " );
			return;
		}

		_task->cancel();
		joinTask_(_task);
	}
	///////////////////////////////////////////////////////////////////////////
	void TaskManager::update()
	{
		this->testComplete_();
		this->injectTasks_();
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class FTaskThreadComplete
		{
		public:
			bool operator () ( TaskThread & taskThread ) const
			{			
				Task * task = taskThread.task;

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
	void TaskManager::testComplete_()
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
		
		TVectorTaskThread::iterator it_remove = 
			std::partition( m_taskThread.begin(), m_taskThread.end(), FTaskThreadComplete());

		for( TVectorTaskThread::iterator 
			it = it_remove,
			it_end = m_taskThread.end();
		it != it_end;
		++it )
		{
			TaskThread & taskThread = *it;

			Task * task = taskThread.task;

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
	void TaskManager::injectTasks_()
	{
		while( m_tasks.empty() == false )
		{
			if( m_taskThread.size() == m_threadCount )
			{
				break;
			}

			Task * task = m_tasks.back();
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

			TaskThread taskThread;

			taskThread.task = task;
			taskThread.identity = threadIdentity;

			m_taskThread.push_back( taskThread );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadIdentity * TaskManager::getThreadIdentity_( Task * _task )
	{
		for( TVectorTaskThread::iterator 
			it = m_taskThread.begin(),
			it_end = m_taskThread.end();
		it != it_end;
		++it )
		{
			TaskThread & taskThread = *it;

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