#	pragma once

#	include "ThreadTask.h"

#	include <vector>

namespace Menge
{
	class ThreadTaskPacket
		: public ThreadTask
	{
	public:
		ThreadTaskPacket();

	public:
		void addTask( const ThreadTaskPtr & _task );
		size_t countTask() const;
		
	protected:
		bool _onMain() override;

	protected:
		bool _onRun() override;
		void _onCancel() override;
		void _onUpdate() override;

	protected:
		void _onComplete( bool _successful ) override;

	protected:
		typedef std::vector<ThreadTaskPtr> TVectorThreadTasks;
		TVectorThreadTasks m_tasks;
	};

	typedef stdex::intrusive_ptr<ThreadTaskPacket> ThreadTaskPacketPtr;
}