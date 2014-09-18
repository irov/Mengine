#	include "MarmaladeThreadIdentity.h"

#	include "Logger/Logger.h"

#   include <unistd.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeThreadIdentity::MarmaladeThreadIdentity()
		: m_serviceProvider(nullptr)
		, m_thread(nullptr)
		, m_task(nullptr)
		, m_complete(true)
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

		return true;
    }	
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeThreadIdentity::main()
	{
		while( true, true )
		{		
			m_mutex->lock();
			if( m_complete == false )
			{				
				m_task->main();
				m_complete = true;
			}
			m_mutex->unlock();

			if( m_exit == true )
			{
				break;
			}

			::usleep( 10 * 1000 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeThreadIdentity::addTask( ThreadTaskInterface * _task )
	{
		if( m_complete == false )
		{
			return false;
		}

		if( m_exit == true )
		{
			return false;				 
		}

		m_task = _task;
		m_complete = false;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeThreadIdentity::joinTask( ThreadTaskInterface * _task )
	{
		if( m_complete == true )
		{
			return false;
		}

		m_mutex->lock();

		if( m_task == _task )
		{
			m_complete = true;
			m_task = nullptr;
		}

		m_mutex->unlock();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeThreadIdentity::join()
	{
		m_exit = true;

		ThreadTaskInterface * join_task = nullptr;
		s3eResult result = s3eThreadJoin( m_thread, (void**)&join_task );

		if( result == S3E_RESULT_ERROR )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadSystem::joinThread: invalid join thread error code - %s"
				, s3eThreadGetErrorString()
				);						
		}
	}
}