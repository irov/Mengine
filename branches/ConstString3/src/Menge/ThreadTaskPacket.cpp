#	include "ThreadTaskPacket.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskPacket::ThreadTaskPacket( ThreadTaskPacketListener * _listener )
		: m_listener(_listener)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPacket::addTask( ThreadTask * _task )
	{
		_task->addListener( this );

		m_tasks.push_back( _task );	
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorTask & ThreadTaskPacket::getTasks() const
	{
		return m_tasks;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPacket::onTaskRun( ThreadTask * _task )
	{
		//ToDo
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPacket::onTaskComplete( ThreadTask * _task )
	{
		TVectorTask::iterator it_found = std::find( m_tasks.begin(), m_tasks.end(), _task );
		m_tasks.erase( it_found );

		if( m_tasks.empty() == false )
		{
			return;
		}

		m_listener->onTaskPacketComplete( this );
	}
}