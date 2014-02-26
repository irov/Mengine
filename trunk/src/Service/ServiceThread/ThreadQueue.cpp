#	include "ThreadQueue.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadQueue::ThreadQueue()
		: m_serviceProvider(nullptr)
		, m_cancel(false)
		, m_packetSize(1)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadQueue::~ThreadQueue()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadQueue::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadQueue::setThreadCount( size_t _count )
	{
		m_currentTasks.resize( _count );
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadQueue::setPacketSize( size_t _size )
	{
		m_packetSize = _size;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadQueue::addTask( const ThreadTaskInterfacePtr & _task )
	{
		m_threadTasks.push( _task );
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadQueue::cancel()
	{
		for( TVectorCurrentThreadTask::iterator
			it = m_currentTasks.begin(),
			it_end = m_currentTasks.end();
		it != it_end;
		++it )
		{
			ThreadTaskInterfacePtr & currentTask = *it;

			currentTask->cancel();
			currentTask = nullptr;
		}

		m_currentTasks.clear();

		while( m_threadTasks.empty() == false )
		{
			ThreadTaskInterfacePtr threadTask = m_threadTasks.pop();
			threadTask->cancel();
		}

		m_cancel = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadQueue::update()
	{
		if( m_cancel == true )
		{
			return true;
		}

		for( TVectorCurrentThreadTask::iterator
			it = m_currentTasks.begin(),
			it_end = m_currentTasks.end();
		it != it_end;
		++it )
		{
			ThreadTaskInterfacePtr & currentTask = *it;

			this->updateCurrentTask_( currentTask );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadQueue::updateCurrentTask_( ThreadTaskInterfacePtr & _currentTask )
	{
		if( _currentTask == nullptr )
		{
			if( m_threadTasks.empty() == true )
			{
				return;
			}

			ThreadTaskPacketPtr packet = m_factoryPoolTaskPacket.createObjectT();
			size_t packetSize = m_packetSize;
						
			while( m_threadTasks.empty() == false && packetSize > 0 )
			{
				ThreadTaskInterfacePtr threadTask = m_threadTasks.pop();

				if( threadTask->isComplete() == true ||
					threadTask->isCancel() == true )
				{
					continue;
				}

				packet->addTask( threadTask );

				--packetSize;
			}

			if( packet->countTask() > 0 )
			{
				if( THREAD_SERVICE(m_serviceProvider)
					->addTask( packet, 0 ) == false )
				{
					return;
				}

				_currentTask = packet;
			}

			if( _currentTask == nullptr )
			{
				return;
			}
		}

		if( _currentTask->isComplete() == true ||
			_currentTask->isCancel() == true )
		{
			_currentTask = nullptr;

			return;
		}

		return;
	}
}