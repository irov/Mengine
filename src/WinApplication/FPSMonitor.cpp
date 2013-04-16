#	include "FPSMonitor.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static DWORD WINAPI s_threadFrameSignal( LPVOID lpParameter )
	{
		FPSMonitor* monitor = reinterpret_cast<FPSMonitor*>(lpParameter);

        DWORD ret = monitor->threadFrameSignal();

		return ret;
	}
	//////////////////////////////////////////////////////////////////////////
	FPSMonitor::FPSMonitor()
		: m_hFrameSignalEvent(INVALID_HANDLE_VALUE)
		, m_hDestroySignalEvent(INVALID_HANDLE_VALUE)
		, m_hFrameSignalThread(INVALID_HANDLE_VALUE)
		, m_running(true)
		, m_active(false)
		, m_frameTiming(100)	
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void FPSMonitor::initialize()
	{
		m_hFrameSignalEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
		m_hDestroySignalEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
	
		m_hFrameSignalThread = CreateThread(NULL, 0, &s_threadFrameSignal, (LPVOID)this, 0, &m_threadId);
		SetThreadPriority(m_hFrameSignalThread, THREAD_PRIORITY_TIME_CRITICAL);
	}
	//////////////////////////////////////////////////////////////////////////
	void FPSMonitor::finalize()
	{
		if( m_hFrameSignalEvent == INVALID_HANDLE_VALUE )
		{
			return;
		}

		if( m_hDestroySignalEvent == INVALID_HANDLE_VALUE )
		{
			return;
		}

		if( m_hFrameSignalThread == INVALID_HANDLE_VALUE )
		{
			return;
		}

		m_running = false;

		WaitForSingleObject( m_hDestroySignalEvent, INFINITE );
		
		CloseHandle( m_hFrameSignalEvent );
		CloseHandle( m_hDestroySignalEvent );
		CloseHandle( m_hFrameSignalThread );
	}
	//////////////////////////////////////////////////////////////////////////
	void FPSMonitor::setActive( bool _active )
	{
		m_active = _active;
	}
	//////////////////////////////////////////////////////////////////////////
	void FPSMonitor::setFrameTime( float _frameTiming )
	{
		m_frameTiming = _frameTiming;
	}
	//////////////////////////////////////////////////////////////////////////
	void FPSMonitor::monitor()
	{
		if( m_active == true )
		{
			WaitForSingleObject( m_hFrameSignalEvent, INFINITE );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	DWORD WINAPI FPSMonitor::threadFrameSignal()
	{
		while( m_running )
		{
			if( m_active == true )
			{
                unsigned long ms_frameTiming = (unsigned long)m_frameTiming;
				Sleep( ms_frameTiming );
				SetEvent( m_hFrameSignalEvent );
			}
			else
			{
				Sleep( 1000 );
			}
		}

		SetEvent( m_hDestroySignalEvent );

		return 0;
	}
}
