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
						
			ThreadTaskInterfacePtr threadTask = m_threadTasks.pop();

			if( THREAD_SERVICE(m_serviceProvider)
				->addTask( threadTask, 0 ) == false )
			{
				return false;
			}

			m_currentTask = threadTask;
		}

		if( m_currentTask->update() == true || 
			m_currentTask->isComplete() == true ||
			m_currentTask->isCancel() == true )
		{
			m_currentTask = nullptr;

			return false;
		}

		return false;
	}
}