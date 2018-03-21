#pragma once

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/ThreadTaskPacket.h"
#include "Factory/Factory.h"

#include "stdex/stl_list.h"

namespace Mengine 
{
	class ThreadQueue
		: public ServantBase<ThreadQueueInterface>
	{
	public:
		ThreadQueue();
		~ThreadQueue();

	public:
		void setThreadName( const ConstString & _threadName );
		void setThreadCount( uint32_t _count );
		void setPacketSize( uint32_t _size );

	public:
		bool update();

	protected:
		void addTask( const ThreadTaskInterfacePtr & _task ) override;
        			
	protected:
		void cancel() override;

	protected:
		void updateCurrentTask_( ThreadTaskInterfacePtr & _currentTask );

    protected:
		ConstString m_threadName;
		uint32_t m_packetSize;

		typedef stdex::list<ThreadTaskInterfacePtr> TVectorThreadTask;
		TVectorThreadTask m_threadTasks;

		typedef stdex::vector<ThreadTaskInterfacePtr> TVectorCurrentThreadTask;
		TVectorCurrentThreadTask m_currentTasks;

		FactoryPtr m_factoryPoolTaskPacket;

		bool m_cancel;
	};

	typedef stdex::intrusive_ptr<ThreadQueue> ThreadQueuePtr;
}
