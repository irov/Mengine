#	include "Win32ThreadIdentity.h"

#	include "Logger/Logger.h"

#	include <process.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32ThreadIdentity::Win32ThreadIdentity()
        : m_serviceProvider(nullptr)
		, m_handle(INVALID_HANDLE_VALUE)
		, m_hTaskSignalEvent(INVALID_HANDLE_VALUE)
        , m_task(nullptr)
		, m_complete(true)
		, m_exit(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32ThreadIdentity::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * Win32ThreadIdentity::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	static unsigned int __stdcall s_tread_job( void * _userData )
	{
		Win32ThreadIdentity * thread = static_cast<Win32ThreadIdentity*>(_userData);
		
		try
		{
			thread->main();
		}
		catch( const stdex::exception & ex )
		{
			ServiceProviderInterface * serviceProvider = thread->getServiceProvider();

			LOGGER_CRITICAL(serviceProvider)("Win32ThreadIdentity stdex::exception '%s' file %d:%s"
				, ex.doc
				, ex.line
				, ex.file
				);
		}
		catch( const std::exception & ex )
		{
			ServiceProviderInterface * serviceProvider = thread->getServiceProvider();

			LOGGER_CRITICAL(serviceProvider)("Win32ThreadIdentity std::exception '%s'"
				, ex.what()
				);
		}
		catch( ... )
		{
			ServiceProviderInterface * serviceProvider = thread->getServiceProvider();

			LOGGER_CRITICAL(serviceProvider)("Win32ThreadIdentity unsupported exception"
				);
		}

		return 0;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadIdentity::initialize( const ThreadMutexInterfacePtr & _mutex, int _priority )
    {
		m_mutex = _mutex;

		m_handle = (HANDLE)_beginthreadex( NULL, 0, &s_tread_job, (LPVOID)this, 0, NULL );

		if( m_handle == NULL )
		{
			DWORD error_code = GetLastError();

			LOGGER_ERROR(m_serviceProvider)("Win32ThreadIdentity::initialize: invalid create thread error code - %d"
				, error_code
				);

			return false;
		}

		switch( _priority )
		{   
		case -2:
			{
				SetThreadPriority( m_handle, THREAD_PRIORITY_LOWEST );
			}break;
		case -1:
			{
				SetThreadPriority( m_handle, THREAD_PRIORITY_BELOW_NORMAL );
			}break;
		case 0:
			{
				SetThreadPriority( m_handle, THREAD_PRIORITY_NORMAL );
			}break;
		case 1:
			{
				SetThreadPriority( m_handle, THREAD_PRIORITY_ABOVE_NORMAL );
			}break;
		case 2:
			{
				SetThreadPriority( m_handle, THREAD_PRIORITY_HIGHEST );
			}break;
		case 3:
			{
				SetThreadPriority( m_handle, THREAD_PRIORITY_TIME_CRITICAL );
			}break;
		}    

		m_hTaskSignalEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

		if( m_hTaskSignalEvent == NULL )
		{
			return false;
		}

		return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void Win32ThreadIdentity::main()
	{
		bool work = true;

		while( work == true )
		{	
			m_mutex->lock();

			if( m_complete == false )
			{
				m_task->main();

				m_task = nullptr;

				m_complete = true;
			}

			if( m_exit == true )
			{
				work = false;
			}

			m_mutex->unlock();

			if( work == true )
			{
				WaitForSingleObject( m_hTaskSignalEvent, INFINITE );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32ThreadIdentity::processTask( ThreadTaskInterface * _task )
	{
		bool successful = false;

		m_mutex->lock();

		if( m_complete == true && m_exit == false )
		{
			m_task = _task;
			m_complete = false;

			successful = true;
		}

		m_mutex->unlock();

		SetEvent( m_hTaskSignalEvent );
		
		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32ThreadIdentity::joinTask()
	{	
		bool successful = false;

		m_mutex->lock();

		if( m_complete == false )
		{
			m_task = nullptr;

			m_complete = true;

			successful = true;
		}

		m_mutex->unlock();

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32ThreadIdentity::join()
	{
		m_mutex->lock();
		m_exit = true;
		m_mutex->unlock();

		SetEvent( m_hTaskSignalEvent );
		CloseHandle( m_hTaskSignalEvent );

		WaitForSingleObject( m_handle, INFINITE );
		CloseHandle( m_handle );
	}
}