#pragma once

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/ThreadTaskPacket.h"
#include "Factory/Factory.h"

#include "Config/List.h"

namespace Mengine 
{
	class ThreadQueue
		: public ServantBase<ThreadQueueInterface>
	{
	public:
		ThreadQueue();
		~ThreadQueue() override;

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

		typedef List<ThreadTaskInterfacePtr> TVectorThreadTask;
		TVectorThreadTask m_threadTasks;

		typedef Vector<ThreadTaskInterfacePtr> TVectorCurrentThreadTask;
		TVectorCurrentThreadTask m_currentTasks;

		FactoryPtr m_factoryPoolTaskPacket;

		bool m_cancel;
	};

	typedef IntrusivePtr<ThreadQueue> ThreadQueuePtr;
}
