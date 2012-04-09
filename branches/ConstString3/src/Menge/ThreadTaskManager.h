#	pragma once

#	include "Interface/ThreadSystemInterface.h"
#	include "Core/Holder.h"
#	include <vector>

namespace Menge
{
	class ThreadTask;
	class ThreadTaskPacket;

	struct ThreadTaskHandle
	{
		ThreadTask * task;
		ThreadIdentity * identity;
	};

	class ThreadTaskManager
		: public Holder<ThreadTaskManager>
	{
	public:
		ThreadTaskManager( ThreadSystemInterface * _interface, size_t _threadCount );
		virtual ~ThreadTaskManager();

	public:
		void initialize();
		void finalize();

	public:
		size_t taskInProgress() const;
		bool isTaskOnProgress( ThreadTask * _task ) const;

	public:
		void addTask( ThreadTask * _task );
		void joinTask( ThreadTask * _task );
		void cancelTask( ThreadTask * _task );

	public:
		void addTaskPacket( ThreadTaskPacket * _taskPacket );

	public:
		void update();

	protected:
		void testComplete_();
		void injectTasks_();
		void joinTask_( ThreadTask * _task );

		ThreadIdentity * getThreadIdentity_( ThreadTask * _task );

	protected:
		ThreadSystemInterface * m_threadSystem;

		size_t m_threadCount;

		typedef std::vector<ThreadTask *> TVectorTask;
		TVectorTask m_tasks;

		typedef std::vector<ThreadTaskHandle> TVectorThreadTask;
		TVectorThreadTask m_taskThread;
	};
}
