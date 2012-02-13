#	include "TaskPacket.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TaskPacket::TaskPacket( TaskPacketListener * _listener )
		: m_listener(_listener)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskPacket::addTask( Task * _task )
	{
		_task->addListener( this );

		m_tasks.push_back( _task );	
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorTask & TaskPacket::getTasks() const
	{
		return m_tasks;
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskPacket::onTaskRun( Task * _task )
	{
		//ToDo
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskPacket::onTaskComplete( Task * _task )
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