#	pragma once

#	include "Task.h"

#	include <vector>
namespace Menge
{
	class TaskPacket;

	class TaskPacketListener
	{
	public:
		virtual void onTaskPacketComplete( TaskPacket * _paket ) = 0;
	};

	typedef std::vector<Task *> TVectorTask;

	class TaskPacket
		: public TaskListener
	{
	public:
		TaskPacket( TaskPacketListener * _listener );

	public:
		void addTask( Task * _task );

	public:
		const TVectorTask & getTasks() const;

	
	protected:
		void onTaskRun( Task * _task ) override;
		void onTaskComplete( Task * _task ) override;

	protected:	
		TVectorTask m_tasks;

		TaskPacketListener * m_listener;
	};
}