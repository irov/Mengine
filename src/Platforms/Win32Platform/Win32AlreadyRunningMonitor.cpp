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
    bool Win32AlreadyRunningMonitor::initialize( EAlreadyRunningPolicy _policy, const WChar * _windowClassName, const WChar * _projectTitle, bool * const _stop )
    {
        // try to create mutex to sure that we are not running already
        WChar mutexName[MENGINE_MAX_PATH + 1] = {L'\0'};
        StdString::wcscpy( mutexName, L"Mengine_ARM_Mutex_" );
        StdString::wcscat( mutexName, _windowClassName );
        StdString::wcscat( mutexName, _projectTitle );

        HANDLE mutex = ::CreateMutex( NULL, FALSE, mutexName );

        if( mutex == NULL )
        {
            return false;
        }

        DWORD error = ::GetLastError();
        // already running

        if( error != ERROR_ALREADY_EXISTS )
        {
            m_mutex = mutex;

            *_stop = false;

            return true;
        }

        ::CloseHandle( mutex );

        switch( _policy )
        {
        case Mengine::EARP_NONE:
            {
                *_stop = false;
            }break;
        case Mengine::EARP_SETFOCUS:
            {
                HWND otherHwnd = ::FindWindow( _windowClassName, _projectTitle );
                ::SetForegroundWindow( otherHwnd );

                LOGGER_MESSAGE_RELEASE( "FOCUS to other instance of application [%ls]"
                    , _projectTitle
                );

                *_stop = true;

                return true;
            }break;
        case Mengine::EARP_SHOWMESSAGE:
            {
                WChar message[1024 + 1] = {L'\0'};
                MENGINE_SWPRINTF( message, 1024, L"Another instance of '%ls' is already running"
                    , _projectTitle
                );

                ::MessageBoxW( NULL, message, _projectTitle, MB_ICONWARNING );

                *_stop = false;

                return true;
            }break;
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
