#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#	include "Kernel/ThreadTaskPacket.h"
#	include "Factory/FactoryStore.h"

#	include "stdex/queue.h"

namespace Menge 
{
	class ThreadQueue
		: public ThreadQueueInterface
	{
	public:
		ThreadQueue();
		~ThreadQueue();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

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

	private:
		ServiceProviderInterface * m_serviceProvider;
		
		ConstString m_threadName;
		uint32_t m_packetSize;

		typedef stdex::queue<ThreadTaskInterfacePtr> TVectorThreadTask;
		TVectorThreadTask m_threadTasks;

		typedef stdex::vector<ThreadTaskInterfacePtr> TVectorCurrentThreadTask;
		TVectorCurrentThreadTask m_currentTasks;

		typedef FactoryPoolStore<ThreadTaskPacket, 4> TFactoryPoolTaskPacket;
		TFactoryPoolTaskPacket m_factoryPoolTaskPacket;

		bool m_cancel;
	};

	typedef stdex::intrusive_ptr<ThreadQueue> ThreadQueuePtr;
}
