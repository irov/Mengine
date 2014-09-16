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
	static unsigned int __stdcall s_tread_job( void * _userData )
	{
		Win32ThreadIdentity * thread = static_cast<Win32ThreadIdentity*>(_userData);
		
		thread->main();

		return 0;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadIdentity::initialize( ServiceProviderInterface * _serviceProvider, const ThreadMutexInterfacePtr & _mutex, int _priority )
    {
		m_serviceProvider = _serviceProvider;
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
		bool done = false;

		while( done == false )
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
				done = true;
			}

			m_mutex->unlock();

			if( done == false )
			{
				WaitForSingleObject( m_hTaskSignalEvent, INFINITE );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32ThreadIdentity::addTask( ThreadTaskInterface * _task )
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
	bool Win32ThreadIdentity::joinTask( ThreadTaskInterface * _task )
	{	
		(void)_task;

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