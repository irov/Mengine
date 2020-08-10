#include "Win32AlreadyRunningMonitor.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32AlreadyRunningMonitor::Win32AlreadyRunningMonitor()
        : m_mutex( NULL )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32AlreadyRunningMonitor::~Win32AlreadyRunningMonitor()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32AlreadyRunningMonitor::initialize( int32_t _policy, const WChar * _windowClassName, const WChar * _projectTitle )
    {
        // try to create mutex to sure that we are not running already
        WString mutexName = L"MengineAlreadyRunningMonitorMutex_";
        mutexName += _projectTitle;

        m_mutex = ::CreateMutex( NULL, FALSE, mutexName.c_str() );

        DWORD error = ::GetLastError();
        // already running

        if( error != ERROR_ALREADY_EXISTS )
        {
            return true;
        }

        if( _policy == EARP_SETFOCUS )
        {
            HWND otherHwnd = ::FindWindow( _windowClassName, _projectTitle );
            ::SetForegroundWindow( otherHwnd );

            LOGGER_ERROR( "AlreadyRunningMonitor FOCUS to other instance of engine"
            );

            return false;
        }

        if( _policy == EARP_SHOWMESSAGE )
        {
            WString message = WString( L"Another instance of " )
                + _projectTitle
                + WString( L" is already running" );

            ::MessageBoxW( NULL, message.c_str(), _projectTitle, MB_ICONWARNING );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AlreadyRunningMonitor::finalize()
    {
        if( m_mutex != NULL )
        {
            ::CloseHandle( m_mutex );
            m_mutex = NULL;
        }
    }
}
