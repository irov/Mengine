#	include "ThreadJob.h"

#   include <algorithm>

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
		for( size_t i = 0; i != 32; ++i )
		{
			WorkerDesc & desc = m_workers[i];

			if( desc.mutex != nullptr )
			{
				desc.mutex->destroy();
				desc.mutex = nullptr;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////        
	void ThreadJob::initialize( ServiceProviderInterface * _serviceProvider, size_t _sleep )
	{
		m_serviceProvider = _serviceProvider;
		m_sleep = _sleep;

		for( size_t i = 0; i != 32; ++i )
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
	size_t ThreadJob::addWorker( ThreadWorkerInterface * _worker )
	{
		for( size_t i = 0; i != 32; ++i )
		{
			WorkerDesc & desc = m_workers[i];

			if( desc.status != ETS_FREE )
			{
				continue;
			}

			desc.mutex->lock();

			desc.worker = _worker;
			desc.id = ++m_enumerator;
			desc.status = ETS_WORK;

			desc.mutex->unlock();

			return desc.id;
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadJob::removeWorker( size_t _id )
	{
		for( size_t i = 0; i != 32; ++i )
		{
			WorkerDesc & desc = m_workers[i];

			if( desc.id != _id )
			{
				continue;
			}

			desc.mutex->lock();
			ThreadWorkerInterface * worker = desc.worker;
			
			desc.id = 0;
			desc.worker = nullptr;
			desc.status = ETS_FREE;
			
			worker->onDone( _id );

			desc.mutex->unlock();
			
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadJob::_onMain()
	{
		while( this->isInterrupt() == false )
		{   
			for( size_t i = 0; i != 32; ++i )
			{
				WorkerDesc & desc = m_workers[i];

				desc.mutex->lock();

				if( desc.status == ETS_WORK )
				{
					if( desc.worker->onWork( desc.id ) == false )
					{
						desc.status = ETS_DONE;
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
		for( size_t i = 0; i != 32; ++i )
		{
			WorkerDesc & desc = m_workers[i];

			if( desc.status != ETS_DONE )
			{
				continue;
			}

			desc.mutex->lock();
			ThreadWorkerInterface * worker = desc.worker; 
			size_t id = desc.id;
			
			desc.worker = nullptr;
			desc.id = 0;
			desc.status = ETS_FREE;

			worker->onDone( id );
			desc.mutex->unlock();
		}
	}
}
