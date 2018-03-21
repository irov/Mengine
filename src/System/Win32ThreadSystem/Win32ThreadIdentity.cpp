#include "Win32ThreadIdentity.h"

#include "Logger/Logger.h"

#include <process.h>

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	Win32ThreadIdentity::Win32ThreadIdentity()
        : m_file(nullptr)
		, m_line(0)
		, m_handle(INVALID_HANDLE_VALUE)
        , m_task(nullptr)
		, m_complete(true)
		, m_exit(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	static unsigned int __stdcall s_tread_job( void * _userData )
	{
		Win32ThreadIdentity * thread = static_cast<Win32ThreadIdentity*>(_userData);

#	ifndef _DEBUG
		try
#	endif
		{
			thread->main();
		}
#	ifndef _DEBUG
		catch( const std::exception & ex )
		{
            LOGGER_CRITICAL( "Win32ThreadIdentity std::exception exception '%s'"
				, ex.what()
				);
		}
		catch( ... )
		{
            LOGGER_CRITICAL( "Win32ThreadIdentity unknown exception"
				);
		}
#	endif

		return 0;
	}
    //////////////////////////////////////////////////////////////////////////
	bool Win32ThreadIdentity::initialize( const ThreadMutexInterfacePtr & _mutex, int _priority, const char * _file, uint32_t _line )
    {
		m_mutex = _mutex;
		
		m_file = _file;
		m_line = _line;

		m_handle = (HANDLE)_beginthreadex( NULL, 0, &s_tread_job, (LPVOID)this, 0, NULL );

		if( m_handle == NULL )
		{
			DWORD error_code = GetLastError();

			LOGGER_ERROR("Win32ThreadIdentity::initialize: invalid create thread error code - %d"
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

			Sleep( 10 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32ThreadIdentity::processTask( ThreadTaskInterface * _task )
	{
		if( m_mutex->try_lock() == false )
		{
			return false;
		}

		bool successful = false;

		if( m_complete == true && m_exit == false )
		{
			m_task = _task;
			m_complete = false;

			successful = true;
		}

		m_mutex->unlock();

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32ThreadIdentity::completeTask()
	{	
        if( m_exit == true )
        {
            return false;
        }

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
		if( m_exit == true )
		{
			return;
		}

		m_mutex->lock();
		{
			m_exit = true;
		}
		m_mutex->unlock();

		WaitForSingleObject( m_handle, INFINITE );
		CloseHandle( m_handle );
		m_handle = INVALID_HANDLE_VALUE;
	}
}