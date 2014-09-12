#	include "ThreadJob.h"

#	include "Core/MutexGuard.h"

#	include "Logger/Logger.h"

namespace Menge 
{
	//////////////////////////////////////////////////////////////////////////
	ThreadJob::ThreadJob()
		: m_serviceProvider(nullptr)
		, m_sleep(0)
		, m_enumerator(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadJob::~ThreadJob()
	{
		for( size_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
		{
			WorkerDesc & desc = m_workers[i];

			desc.mutex = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////        
	void ThreadJob::initialize( ServiceProviderInterface * _serviceProvider, unsigned int _sleep )
	{
		m_serviceProvider = _serviceProvider;
		m_sleep = _sleep;

		for( size_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
		{
			WorkerDesc & desc = m_workers[i];
			
			desc.mutex = THREAD_SERVICE(m_serviceProvider)
				->createMutex();

			desc.worker = nullptr;
			desc.id = 0;
			desc.status = ETS_FREE;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ThreadJob::addWorker( const ThreadWorkerInterfacePtr & _worker )
	{
		if( _worker == nullptr )
		{
			return 0;
		}

		for( size_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
		{
			WorkerDesc & desc = m_workers[i];

			if( desc.status != ETS_FREE )
			{
				continue;
			}

			
			desc.mutex->lock();
			{
				desc.worker = _worker;
				desc.id = ++m_enumerator;
				desc.status = ETS_WORK;
			}
			desc.mutex->unlock();

			return desc.id;
		}

		LOGGER_ERROR(m_serviceProvider)("ThreadJob::addWorker overworkers more %d"
			, MENGINE_THREAD_JOB_WORK_COUNT
			);

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadJob::removeWorker( size_t _id )
	{
		if( _id == 0 )
		{
			return;
		}

		for( size_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
		{
			WorkerDesc & desc = m_workers[i];

			if( desc.id != _id )
			{
				continue;
			}

			desc.mutex->lock();
			{
				ThreadWorkerInterfacePtr worker = desc.worker;

				desc.id = 0;
				desc.worker = nullptr;
				desc.status = ETS_FREE;

				worker->onDone( _id );
			}
			desc.mutex->unlock();
			
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadJob::_onMain()
	{
		while( this->isCancel() == false )
		{   
			for( size_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
			{
				WorkerDesc & desc = m_workers[i];

				if( desc.status != ETS_WORK )
				{
					continue;
				}
				
				desc.mutex->lock();
				{
					if( desc.status == ETS_WORK )
					{
						if( desc.worker->onWork( desc.id ) == false )
						{
							desc.status = ETS_DONE;
						}
					}
				}	
				desc.mutex->unlock();
			}

			THREAD_SERVICE(m_serviceProvider)
				->sleep( m_sleep );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadJob::_onUpdate()
	{
		for( size_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
		{
			WorkerDesc & desc = m_workers[i];

			if( desc.status != ETS_DONE )
			{
				continue;
			}

			desc.mutex->lock();
			{
				ThreadWorkerInterfacePtr worker = desc.worker; 
				size_t id = desc.id;

				desc.worker = nullptr;
				desc.id = 0;
				desc.status = ETS_FREE;

				worker->onDone( id );
			}
			desc.mutex->unlock();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadJob::destroy()
	{
		delete this;
	}
}
