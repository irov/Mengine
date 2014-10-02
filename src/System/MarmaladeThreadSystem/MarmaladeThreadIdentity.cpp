#	include "MarmaladeThreadIdentity.h"

#	include "Logger/Logger.h"

#	include "s3eDevice.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeThreadIdentity::MarmaladeThreadIdentity()
		: m_serviceProvider(nullptr)
		, m_thread(nullptr)
		, m_task(nullptr)
		, m_complete(true)
		, m_join(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	static void * s_tread_job( void * _userData )
	{
		MarmaladeThreadIdentity * thread = static_cast<MarmaladeThreadIdentity *>(_userData);
		
		thread->main();

		return 0;
	}
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeThreadIdentity::initialize( ServiceProviderInterface * _serviceProvider, const ThreadMutexInterfacePtr & _mutex, int _priority )
    {
		m_serviceProvider = _serviceProvider;
		m_mutex = _mutex;

		s3eThread * s3e_thread = s3eThreadCreate( &s_tread_job, (void *)this, nullptr );

		if( s3e_thread == nullptr )
		{			
			LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadSystem::createThread: invalid create thread error code - %s"
				, s3eThreadGetErrorString()
				);

			return false;
		}

		m_thread = s3e_thread;

		return true;
    }	
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeThreadIdentity::main()
	{
		bool work = true;
		while( work )
		{		
			m_mutex->lock();
			{
				if( m_complete == false )
				{				
					m_task->main();
					m_complete = true;
				}

				if( m_exit == true )
				{
					work = false;
				}
			}
			m_mutex->unlock();
			
			s3eDeviceYield( 10 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeThreadIdentity::addTask( ThreadTaskInterface * _task )
	{
		bool successful = true;
		m_mutex->lock();
		{
			if( m_complete == false )
			{
				successful = false;
			}

			if( m_exit == true )
			{
				successful = false;
			}
		}
		m_mutex->unlock();

		if( successful == false )
		{
			return false;
		}

		m_mutex->lock();
		{
			m_task = _task;
			m_complete = false;
		}
		m_mutex->unlock();


		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeThreadIdentity::joinTask( ThreadTaskInterface * _task )
	{
		bool successful = false;

		m_mutex->lock();

		if( m_complete == false )
		{			
			if( m_task == _task )
			{
				m_complete = true;
				m_task = nullptr;

				successful = true;
			}
		}

		m_mutex->unlock();

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeThreadIdentity::join()
	{
		if( m_join == true )
		{
			return;
		}

		m_mutex->lock();
		{
			m_exit = true;
		}
		m_mutex->unlock();

		ThreadTaskInterface * join_task = nullptr;
		s3eResult result = s3eThreadJoin( m_thread, (void**)&join_task );

		if( result == S3E_RESULT_ERROR )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadSystem::joinThread: invalid join thread error code - %s"
				, s3eThreadGetErrorString()
				);						
		}

		m_join = true;
	}
}