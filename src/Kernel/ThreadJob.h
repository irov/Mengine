#	pragma once

#	include "Kernel/ThreadTask.h"

#   include <list>

#	ifndef MENGINE_THREAD_JOB_WORK_COUNT
#	define MENGINE_THREAD_JOB_WORK_COUNT 32
#	endif

namespace Menge 
{
	class ThreadWorkerInterface
		: public FactorablePtr
	{
	public:
		virtual bool onWork( size_t _id ) = 0;
		virtual void onDone( size_t _id ) = 0;
	};

	typedef stdex::intrusive_ptr<ThreadWorkerInterface> ThreadWorkerInterfacePtr;

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
		void initialize( ServiceProviderInterface * _serviceProvider, unsigned int _sleep );

	public:
		size_t addWorker( const ThreadWorkerInterfacePtr &_worker );
		void removeWorker( size_t _id );

	protected:
		bool _onMain() override;
		void _onUpdate() override;

	protected:
		void destroy() override;

	protected:
		bool check_remove( size_t _id );

	public:
		struct WorkerDesc
		{
			ThreadMutexInterfacePtr mutex;

			ThreadWorkerInterfacePtr worker;

			size_t id;
			EThreadStatus status;
		};

	protected:
		ServiceProviderInterface * m_serviceProvider;
		unsigned int m_sleep;
  
		size_t m_enumerator;
		
		WorkerDesc m_workers[MENGINE_THREAD_JOB_WORK_COUNT];
	};

	typedef stdex::intrusive_ptr<ThreadJob> ThreadJobPtr;
}
