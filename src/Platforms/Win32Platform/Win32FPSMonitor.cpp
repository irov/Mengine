#include "Win32FPSMonitor.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static DWORD WINAPI s_threadFrameSignal( LPVOID lpParameter )
    {
        Win32FPSMonitor * monitor = reinterpret_cast<Win32FPSMonitor *>(lpParameter);

        DWORD ret = monitor->threadFrameSignal();

        return ret;
    }
    //////////////////////////////////////////////////////////////////////////
    Win32FPSMonitor::Win32FPSMonitor()
        : m_hFrameSignalEvent( INVALID_HANDLE_VALUE )
        , m_hDestroySignalEvent( INVALID_HANDLE_VALUE )
        , m_hFrameSignalThread( INVALID_HANDLE_VALUE )
        , m_running( true )
        , m_active( false )
        , m_frameTime( 100 )
        , m_threadId( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32FPSMonitor::~Win32FPSMonitor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FPSMonitor::initialize()
    {
        m_hFrameSignalEvent = ::CreateEvent( NULL, FALSE, FALSE, NULL );
        m_hDestroySignalEvent = ::CreateEvent( NULL, FALSE, FALSE, NULL );

        m_hFrameSignalThread = ::CreateThread( NULL, 0, &s_threadFrameSignal, (LPVOID)this, 0, &m_threadId );

        if( m_hFrameSignalThread == 0 )
        {
            return false;
        }

        ::SetThreadPriority( m_hFrameSignalThread, THREAD_PRIORITY_TIME_CRITICAL );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FPSMonitor::finalize()
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

        ::WaitForSingleObject( m_hDestroySignalEvent, INFINITE );

        ::CloseHandle( m_hFrameSignalEvent );
        ::CloseHandle( m_hDestroySignalEvent );
        ::CloseHandle( m_hFrameSignalThread );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FPSMonitor::setActive( bool _active )
    {
        m_active = _active;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FPSMonitor::setFrameTime( float _frameTiming )
    {
        m_frameTime = _frameTiming;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FPSMonitor::monitor()
    {
        if( m_active == true )
        {
            ::WaitForSingleObject( m_hFrameSignalEvent, INFINITE );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    DWORD WINAPI Win32FPSMonitor::threadFrameSignal()
    {
        while( m_running )
        {
            if( m_active == true )
            {
                unsigned long ms_frameTiming = (unsigned long)m_frameTime;
                ::Sleep( ms_frameTiming );
                ::SetEvent( m_hFrameSignalEvent );
            }
            else
            {
                ::Sleep( 1000 );
            }
        }

        ::SetEvent( m_hDestroySignalEvent );

        return 0;
    }
}
