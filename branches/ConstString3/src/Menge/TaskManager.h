#	pragma once

#	include "Interface/ThreadSystemInterface.h"
#	include "Core/Holder.h"
#	include <vector>

namespace Menge
{
	class Task;
	class TaskPacket;

	class TaskManager
		: public Holder<TaskManager>
	{
	public:
		TaskManager( ThreadSystemInterface * _interface, size_t _threadCount );
		virtual ~TaskManager();
	public:
		void initialize();
		void finalize();

	public:
		size_t taskInProgress() const;
		bool isTaskOnProgress( Task * _task ) const;
	public:
		void addTask( Task * _task );
		void joinTask( Task * _task );
		void cancelTask( Task * _task );
	public:
		void addTaskPacket( TaskPacket * _taskPacket );

	public:
		void update();

	protected:
		void testComplete_();
		void injectTasks_();
		void joinTask_( Task * _task );
		ThreadIdentity * getThreadIdentity_( Task * _task );

	protected:
		ThreadSystemInterface * m_threadSystem;

		size_t m_threadCount;

		typedef std::vector<Task *> TVectorTask;
		TVectorTask m_tasks;

		struct TaskThread
		{
			Task * task;
			ThreadIdentity * identity;
		};

		typedef std::vector<TaskThread> TVectorTaskThread;
		TVectorTaskThread m_taskThread;
	};
}
