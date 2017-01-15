#	pragma once

#	include "Kernel/ThreadTask.h"

#   include <list>

#	ifndef MENGINE_THREAD_JOB_WORK_COUNT
#	define MENGINE_THREAD_JOB_WORK_COUNT 32
#	endif

namespace Menge 
{
	class ThreadWorkerInterface
		: public ServantInterface
	{
	public:
		virtual bool onWork( uint32_t _id ) = 0;
		virtual void onDone( uint32_t _id ) = 0;
	};

	typedef stdex::intrusive_ptr<ThreadWorkerInterface> ThreadWorkerInterfacePtr;

	enum EThreadStatus
	{
		ETS_WORK,
		ETS_DONE,
		ETS_FREE
	};

	struct ThreadJobWorkerDesc
	{
		ThreadMutexInterfacePtr mutex;

		ThreadWorkerInterfacePtr worker;

		volatile uint32_t id;

		volatile uint32_t status;
	};

	class ThreadJob
		: public ThreadTask
	{
	public:
		ThreadJob();
		~ThreadJob();

	public:
		void initialize( unsigned int _sleep );

	public:
		uint32_t addWorker( const ThreadWorkerInterfacePtr &_worker );
		void removeWorker( uint32_t _id );

	protected:
		bool _onMain() override;
		void _onUpdate() override;

	protected:
		void destroy() override;

	protected:
		bool check_remove( uint32_t _id );

	protected:
		unsigned int m_sleep;
  
		uint32_t m_enumerator;
		
		ThreadJobWorkerDesc m_workers[MENGINE_THREAD_JOB_WORK_COUNT];
	};

	typedef stdex::intrusive_ptr<ThreadJob> ThreadJobPtr;
}
