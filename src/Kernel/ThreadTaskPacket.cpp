#include "ThreadTaskPacket.h"

#include "Logger/Logger.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskPacket::ThreadTaskPacket()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskPacket::initialize( uint32_t _packetSize )
	{
		m_tasks.reserve( _packetSize );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPacket::addTask( const ThreadTaskPtr & _task )
	{
		m_tasks.emplace_back( _task );
	}
	//////////////////////////////////////////////////////////////////////////
	const ThreadTaskPtr & ThreadTaskPacket::getTask( uint32_t _index ) const
	{
		return m_tasks[_index];
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t ThreadTaskPacket::countTask() const
	{
		uint32_t count = (uint32_t)m_tasks.size();

		return count;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskPacket::_onMain()
	{
		for( TVectorThreadTasks::iterator
			it = m_tasks.begin(),
			it_end = m_tasks.end();
		it != it_end;
		++it )
		{
			const ThreadTaskPtr & task = *it;

			task->main();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskPacket::_onRun()
	{
		for( TVectorThreadTasks::iterator
			it = m_tasks.begin(),
			it_end = m_tasks.end();
		it != it_end;
		++it )
		{
			const ThreadTaskPtr & task = *it;

			task->run();
		}

		return true;			 
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPacket::_onCancel()
	{
		for( TVectorThreadTasks::iterator
			it = m_tasks.begin(),
			it_end = m_tasks.end();
		it != it_end;
		++it )
		{
			const ThreadTaskPtr & task = *it;

			task->cancel();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPacket::_onUpdate()
	{
		for( TVectorThreadTasks::iterator
			it = m_tasks.begin(),
			it_end = m_tasks.end();
		it != it_end;
		++it )
		{
			const ThreadTaskPtr & task = *it;

			if( task->isCancel() == true ||
				task->isComplete() == true )
			{
				continue;
			}

			task->update();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPacket::_onComplete( bool _successful )
	{
		(void)_successful;

		m_tasks.clear();
	}
}