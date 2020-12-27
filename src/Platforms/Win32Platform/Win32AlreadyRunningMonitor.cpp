#include "Win32AlreadyRunningMonitor.h"

#include "Kernel/Logger.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

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
        WChar mutexName[MENGINE_MAX_PATH] = {L'\0'};
        MENGINE_WCSCPY( mutexName, L"MengineAlreadyRunningMonitorMutex_" );
        MENGINE_WCSCAT( mutexName, _projectTitle );

        m_mutex = ::CreateMutex( NULL, FALSE, mutexName );

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

            LOGGER_MESSAGE_RELEASE( "AlreadyRunningMonitor FOCUS to other instance of engine" );

            return false;
        }

        if( _policy == EARP_SHOWMESSAGE )
        {
            WChar message[1024] = {L'\0'};
            MENGINE_WNSPRINTF( message, 1024, L"Another instance of '%ls' is already running"
                , _projectTitle 
            );

            ::MessageBoxW( NULL, message, _projectTitle, MB_ICONWARNING );

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
    //////////////////////////////////////////////////////////////////////////
}
