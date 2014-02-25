#	include "ThreadPool.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadPool::ThreadPool()
		: m_serviceProvider(nullptr)
		, m_cancel(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadPool::~ThreadPool()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadPool::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadPool::addTask( const ThreadTaskInterfacePtr & _task )
	{
		m_threadTasks.push( _task );
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadPool::cancel()
	{
		if( m_currentTask != nullptr )
		{
			m_currentTask->cancel();
			m_currentTask = nullptr;
		}

		m_threadTasks.clear();
		m_cancel = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadPool::update()
	{
		if( m_cancel == true )
		{
			return true;
		}

		if( m_currentTask == nullptr )
		{
			if( m_threadTasks.empty() == true )
			{
				return false;
			}
						
			while( m_threadTasks.empty() == false )
			{
				ThreadTaskInterfacePtr threadTask = m_threadTasks.pop();

				if( threadTask->isComplete() == true ||
					threadTask->isCancel() == true )
				{
					continue;
				}

				if( THREAD_SERVICE(m_serviceProvider)
					->addTask( threadTask, 0 ) == false )
				{
					return false;
				}

				m_currentTask = threadTask;

				break;
			}

			if( m_currentTask == nullptr )
			{
				return false;
			}
		}

		if( m_currentTask->isComplete() == true ||
			m_currentTask->isCancel() == true )
		{
			m_currentTask = nullptr;

			return false;
		}

		return false;
	}
}