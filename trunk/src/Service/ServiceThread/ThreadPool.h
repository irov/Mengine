#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#	include "stdex/queue.h"

namespace Menge 
{
	class ThreadPool
		: public ThreadPoolInterface
	{
	public:
		ThreadPool();
		~ThreadPool();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		bool update();

	protected:
		void addTask( const ThreadTaskInterfacePtr & _task ) override;
			
	protected:
		void cancel() override;

	private:
		ServiceProviderInterface * m_serviceProvider;

		typedef stdex::queue<ThreadTaskInterfacePtr> TVectorThreadTask;
		TVectorThreadTask m_threadTasks;

		ThreadTaskInterfacePtr m_currentTask;

		bool m_cancel;
	};

	typedef stdex::intrusive_ptr<ThreadPool> ThreadPoolPtr;
}
