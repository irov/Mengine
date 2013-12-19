#	pragma once

#	include "Kernel/ThreadTask.h"

#   include <list>

#	ifndef MENGINE_THREAD_JOB_WORK_COUNT
#	define MENGINE_THREAD_JOB_WORK_COUNT 32
#	endif

namespace Menge 
{
	class ThreadWorkerInterface
		: public Factorable
	{
	public:
		virtual bool onWork( size_t _id ) = 0;
		virtual void onDone( size_t _id ) = 0;
	};

	enum EThreadStatus
	{
		ETS_WORK,
		ETS_DONE,
		ETS_FREE
	};

	class ThreadJob
		: public ThreadTask
	{
	public:
		ThreadJob();
		~ThreadJob();

	public:
		void initialize( ServiceProviderInterface * _serviceProvider, size_t _sleep );

	public:
		size_t addWorker( ThreadWorkerInterface * _worker );
		void removeWorker( size_t _id );

	protected:
		bool _onMain() override;
		void _onUpdate() override;

	protected:
		bool check_remove( size_t _id );

	public:
		struct WorkerDesc
		{
			ThreadMutexInterface * mutex;

			ThreadWorkerInterface * worker;			
			size_t id;
			EThreadStatus status;
		};

	protected:
		ServiceProviderInterface * m_serviceProvider;
		size_t m_sleep;
  
		size_t m_enumerator;
		
		WorkerDesc m_workers[MENGINE_THREAD_JOB_WORK_COUNT];
	};
}
