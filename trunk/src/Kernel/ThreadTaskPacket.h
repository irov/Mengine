#	pragma once

#	include "ThreadTask.h"

#	include <vector>

namespace Menge
{
	class ThreadTaskPacket;

	class ThreadTaskPacketListener
	{
	public:
		virtual void onTaskPacketComplete( ThreadTaskPacket * _paket ) = 0;
	};

	class ThreadTaskPacket
		: public ThreadTaskListener
	{
	public:
		ThreadTaskPacket( ThreadTaskPacketListener * _listener );

	public:
		void addTask( ThreadTask * _task );

	public:
		const TVectorThreadTask & getTasks() const;

	
	protected:
		void onTaskRun( ThreadTask * _task ) override;
		void onTaskComplete( ThreadTask * _task ) override;

	protected:	
		TVectorThreadTask m_tasks;

		ThreadTaskPacketListener * m_listener;
	};
}