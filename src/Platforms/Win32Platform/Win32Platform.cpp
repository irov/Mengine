#include "Win32Platform.h"

#include "Interface/OptionsServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/TimeSystemInterface.h"

#include "Win32DynamicLibrary.h"
#include "Win32AntifreezeMonitor.h"
#include "Win32CriticalErrorsMonitor.h"

#include "Kernel/PathHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/Stringalized.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Config/Stringstream.h"

#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(push, 0) 
#endif

#include "Environment/Windows/DbgHelp.h"

#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(pop) 
#endif

#include <cstdio>
#include <clocale>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <functional>

#include <errno.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Platform, Mengine::Win32Platform );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_WINDOW_CLASSNAME
#define MENGINE_WINDOW_CLASSNAME (L"MengineWindow")
#endif
    //////////////////////////////////////////////////////////////////////////
    Win32Platform::Win32Platform()
        : m_hInstance( NULL )
        , m_hWnd( NULL )
        , m_active( false )
        , m_update( false )
        , m_icon( 0 )
        , m_close( false )
        , m_vsync( false )
        , m_pauseUpdatingTime( -1.f )
        , m_activeFrameTime( 0.f )
        , m_inactiveFrameTime( 100.f )
        , m_prevTime( 0 )
        , m_cursorInArea( false )
        , m_clickOutArea( false )
        , m_isDoubleClick( false )
        , m_cursorMode( false )
        , m_cursor( nullptr )
        , m_lastMouse( false )
        , m_lastMouseX( 0 )
        , m_lastMouseY( 0 )
        , m_touchpad( false )
        , m_fullscreen( false )
    {
        m_projectTitle[0] = '\0';
    }
    //////////////////////////////////////////////////////////////////////////
    Win32Platform::~Win32Platform()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::_initializeService()
    {
#if defined(MENGINE_DEBUG)
        {
            bool developmentMode = HAS_OPTION( "dev" );
            bool roamingMode = HAS_OPTION( "roaming" );
            bool noroamingMode = HAS_OPTION( "noroaming" );

            if( developmentMode == true && (roamingMode == false || noroamingMode == true) )
            {
                Char userPath[MENGINE_MAX_PATH] = { 0 };
                this->getUserPath( userPath );

                String dumpPath;
                dumpPath += userPath;
                dumpPath += "Dump";
                dumpPath += "_";

                PlatformDateTime dateTime;
                this->getDateTime( &dateTime );

                Stringstream ss_date;
                ss_date << dateTime.year
                    << "_" << std::setw( 2 ) << std::setfill( '0' ) << (dateTime.month)
                    << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.day
                    << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.hour
                    << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.minute
                    << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.second;

                String str_date = ss_date.str();

                dumpPath += str_date;
                dumpPath += ".dmp";

                Win32CriticalErrorsMonitor::run( dumpPath.c_str() );
            }
        }
#endif

        SYSTEMTIME tm;
        GetLocalTime( &tm );
        LOGGER_MESSAGE( "Date: %02d.%02d.%d, %02d:%02d:%02d"
            , tm.wDay
            , tm.wMonth
            , tm.wYear
            , tm.wHour
            , tm.wMinute
            , tm.wSecond
        );

        MEMORYSTATUSEX mem_st;
        if( GlobalMemoryStatusEx( &mem_st ) == TRUE )
        {
            LOGGER_MESSAGE( "Memory: %uK total, %uK free, %uK Page file total, %uK Page file free"
                , (uint32_t)(mem_st.ullTotalPhys / 1024UL)
                , (uint32_t)(mem_st.ullAvailPhys / 1024UL)
                , (uint32_t)(mem_st.ullTotalPageFile / 1024UL)
                , (uint32_t)(mem_st.ullAvailPageFile / 1024UL)
            );
        }

        if( this->setProcessDPIAware() == false )
        {
            LOGGER_ERROR( "Application not setup Process DPI Aware"
            );
        }

        m_hInstance = GetModuleHandle( NULL );

        constexpr HashType hash = Helper::makeHash( "IDC_ARROW", (sizeof( "IDC_ARROW" ) - 1) );
        Helper::stringizeStringSizeHash( "IDC_ARROW", (sizeof( "IDC_ARROW" ) - 1), hash );

        STRINGIZE_STRING_LOCAL( "IDC_ARROW" );

        m_cursors[STRINGIZE_STRING_LOCAL( "IDC_ARROW" )] = LoadCursor( NULL, IDC_ARROW );
        m_cursors[STRINGIZE_STRING_LOCAL( "IDC_UPARROW" )] = LoadCursor( NULL, IDC_UPARROW );
        m_cursors[STRINGIZE_STRING_LOCAL( "IDC_HAND" )] = LoadCursor( NULL, IDC_HAND );
        m_cursors[STRINGIZE_STRING_LOCAL( "IDC_HELP" )] = LoadCursor( NULL, IDC_HELP );

        m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );

#ifndef _WIN64
        m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN32" ) );
#else
        m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN64" ) );
#endif

        m_touchpad = false;

        if( HAS_OPTION( "win32" ) )
        {
            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN32" ) );

            m_touchpad = false;
        }
        else if( HAS_OPTION( "win64" ) )
        {
            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN64" ) );

            m_touchpad = false;
        }
        else if( HAS_OPTION( "mac" ) )
        {
            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "MAC" ) );

            m_touchpad = false;
        }
        else if( HAS_OPTION( "ios" ) )
        {
            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "IOS" ) );

#ifdef _WIN32
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN32" ) );
#endif

            m_touchpad = true;
        }
        else if( HAS_OPTION( "android" ) )
        {
            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "ANDROID" ) );

            m_touchpad = true;
        }
        else if( HAS_OPTION( "wp" ) )
        {
            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WP" ) );

            m_touchpad = true;
        }

        const Char * option_platform = GET_OPTION_VALUE( "platform" );

        if( option_platform != nullptr )
        {
            m_platformTags.clear();
            m_platformTags.addTag( Helper::stringizeString( option_platform ) );
        }

        if( HAS_OPTION( "touchpad" ) )
        {
            m_touchpad = true;
        }

        m_antifreezeMonitor = Helper::makeFactorableUnique<Win32AntifreezeMonitor>();

        m_factoryDynamicLibraries = new FactoryPool<Win32DynamicLibrary, 8>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::_finalizeService()
    {
        this->setActive_( false );

        m_platformTags.clear();

        m_cursors.clear();

        if( m_alreadyRunningMonitor != nullptr )
        {
            m_alreadyRunningMonitor->finalize();
            m_alreadyRunningMonitor = nullptr;
        }

        if( m_antifreezeMonitor != nullptr )
        {
            m_antifreezeMonitor->finalize();
            m_antifreezeMonitor = nullptr;
        }

        if( m_hWnd != NULL )
        {
            ::CloseWindow( m_hWnd );
            ::DestroyWindow( m_hWnd );

            m_hWnd = NULL;
        }

        if( m_hInstance != NULL )
        {
            if( ::UnregisterClass( MENGINE_WINDOW_CLASSNAME, m_hInstance ) == FALSE )
            {
                LOGGER_ERROR( "invalid UnregisterClass '%s'"
                    , MENGINE_WINDOW_CLASSNAME
                );
            }

            m_hInstance = NULL;
        }

        if( m_fpsMonitor != nullptr )
        {
            m_fpsMonitor->finalize();
            m_fpsMonitor = nullptr;
        }

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDynamicLibraries );

        m_factoryDynamicLibraries = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::setProcessDPIAware()
    {
        HMODULE hUser32 = ::LoadLibrary( L"user32.dll" );

        if( hUser32 == NULL )
        {
            return false;
        }

        typedef BOOL( WINAPI * PSETPROCESSDPIAWARE )(VOID);

        FARPROC pSetProcessDPIAware =
            ::GetProcAddress( hUser32, "SetProcessDPIAware" );

        if( pSetProcessDPIAware == NULL )
        {
            ::FreeLibrary( hUser32 );

            return false;
        }

        if( pSetProcessDPIAware() == FALSE )
        {
            ::FreeLibrary( hUser32 );

            return false;
        }

        ::FreeLibrary( hUser32 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::createProcessDump( const Char * _dumpPath, void * _pExceptionPointers, bool _full )
    {
#if defined(MENGINE_DEBUG)
        if( IsDebuggerPresent() == TRUE )
        {
            return false;
        }

        WString unicode_processDumpPath;
        Helper::utf8ToUnicode( _dumpPath, unicode_processDumpPath );

        HANDLE hFile = ::CreateFile( unicode_processDumpPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0 );

        if( hFile == INVALID_HANDLE_VALUE )
        {
            return false;
        }

        HMODULE dbghelp_dll = ::LoadLibrary( L"dbghelp.dll" );

        if( dbghelp_dll == NULL )
        {
            ::CloseHandle( hFile );

            return false;
        }

        typedef BOOL( WINAPI * MINIDUMPWRITEDUMP )(
            HANDLE hprocess, DWORD pid, HANDLE hfile, MINIDUMP_TYPE dumptype,
            CONST PMINIDUMP_EXCEPTION_INFORMATION exceptionparam,
            CONST PMINIDUMP_USER_STREAM_INFORMATION userstreamparam,
            CONST PMINIDUMP_CALLBACK_INFORMATION callbackparam
            );

        MINIDUMPWRITEDUMP MiniDumpWriteDump = (MINIDUMPWRITEDUMP)::GetProcAddress( dbghelp_dll, "MiniDumpWriteDump" );

        if( MiniDumpWriteDump == NULL )
        {
            ::FreeLibrary( dbghelp_dll );
            ::CloseHandle( hFile );

            return false;
        }


        MINIDUMP_EXCEPTION_INFORMATION exinfo;

        exinfo.ThreadId = ::GetCurrentThreadId();
        exinfo.ExceptionPointers = (PEXCEPTION_POINTERS)_pExceptionPointers;
        exinfo.ClientPointers = TRUE;

        HANDLE hProcess = ::GetCurrentProcess();
        DWORD dwProcessId = ::GetCurrentProcessId();

        MINIDUMP_TYPE dumptype;

        if( _full == false )
        {
            dumptype = MINIDUMP_TYPE( MiniDumpNormal | MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithDataSegs | MiniDumpWithThreadInfo );
        }
        else
        {
            dumptype = MINIDUMP_TYPE( MiniDumpWithFullMemory | MiniDumpWithFullMemoryInfo | MiniDumpWithHandleData | MiniDumpWithUnloadedModules | MiniDumpWithThreadInfo );
        }

        BOOL successful = MiniDumpWriteDump( hProcess, dwProcessId, hFile, dumptype, (_pExceptionPointers == nullptr ? nullptr : &exinfo), NULL, NULL );

        ::FreeLibrary( dbghelp_dll );
        ::CloseHandle( hFile );

        if( successful == FALSE )
        {
            return false;
        }

        return true;
#endif

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::runPlatform()
    {
        bool developmentMode = HAS_OPTION( "dev" );

        if( developmentMode == true )
        {
            if( m_antifreezeMonitor->initialize() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::updatePlatform()
    {
        this->setActive_( true );

        m_prevTime = TIME_SYSTEM()
            ->getTimeMilliseconds();

        bool developmentMode = HAS_OPTION( "dev" );

        {
            while( m_close == false )
            {
                uint64_t currentTime = TIME_SYSTEM()
                    ->getTimeMilliseconds();

                float frameTime = (float)(currentTime - m_prevTime);

                m_prevTime = currentTime;

                if( developmentMode == true )
                {
                    m_antifreezeMonitor->ping();
                }

                MSG  msg;
                while( ::PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != FALSE )
                {
                    ::TranslateMessage( &msg );
                    ::DispatchMessage( &msg );
                }

                m_update = true;

                bool updating = APPLICATION_SERVICE()
                    ->beginUpdate();

                if( updating == true )
                {
                    if( m_pauseUpdatingTime >= 0.f )
                    {
                        frameTime = m_pauseUpdatingTime;
                        m_pauseUpdatingTime = -1.f;
                    }

                    APPLICATION_SERVICE()
                        ->tick( frameTime );

                    if( m_fpsMonitor != nullptr )
                    {
                        m_fpsMonitor->monitor();
                    }
                    else
                    {
                        ::Sleep( 0 );
                    }
                }
                else
                {
                    ::Sleep( 100 );
                }

                bool focus = APPLICATION_SERVICE()
                    ->isFocus();

                bool nopause = APPLICATION_SERVICE()
                    ->getNopause();

                if( focus == true && m_active == true || nopause == true )
                {
                    bool sucessful = APPLICATION_SERVICE()
                        ->render();

                    if( sucessful == true )
                    {
                        APPLICATION_SERVICE()
                            ->flush();
                    }
                }
                else
                {
                    if( m_pauseUpdatingTime < 0.f )
                    {
                        m_pauseUpdatingTime = frameTime;
                    }

                    ::Sleep( 100 );
                }

                APPLICATION_SERVICE()
                    ->endUpdate();

                m_update = false;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::stopPlatform()
    {
        LOGGER_MESSAGE( "stop platform" );

        bool developmentMode = HAS_OPTION( "dev" );

        if( developmentMode == true )
        {
            if( m_antifreezeMonitor != nullptr )
            {
                m_antifreezeMonitor->finalize();
                m_antifreezeMonitor = nullptr;
            }
        }

        m_mouseEvent.stop();

        if( m_hWnd != NULL )
        {
            ::CloseWindow( m_hWnd );
            ::DestroyWindow( m_hWnd );

            m_hWnd = NULL;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::setIcon( uint32_t _icon )
    {
        m_icon = _icon;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Win32Platform::getIcon() const
    {
        return m_icon;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::setProjectTitle( const Char * _projectTitle )
    {
        Helper::utf8ToUnicode( _projectTitle, m_projectTitle, MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32Platform::getProjectTitle( Char * _projectTitle ) const
    {
        size_t projectTitleLen = Helper::unicodeToUtf8( m_projectTitle, _projectTitle, MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME );

        return projectTitleLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32Platform::getShortPathName( const Char * _path, Char * _shortpath, size_t _len ) const
    {
        WChar unicode_path[MENGINE_MAX_PATH];
        if( Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR( "invalid convert path to unicode" );

            return 0;
        }

        WChar unicode_shortpath[MENGINE_MAX_PATH] = { 0 };
        DWORD len = ::GetShortPathName( unicode_path, unicode_shortpath, (DWORD)_len );

        if( Helper::unicodeToUtf8Size( unicode_shortpath, (size_t)len, _shortpath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR( "invalid convert shortpath to utf8" );

            return 0;
        }

        return (size_t)len;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32Platform::getSystemFontPath( const Char * _fontName, Char * _fontPath ) const
    {   
        WChar unicode_fontName[MAX_PATH];
        if( Helper::utf8ToUnicode( _fontName, unicode_fontName, MAX_PATH ) == false )
        {
            LOGGER_ERROR( "invalid convert fontName '%s' to unicode"
                , _fontName 
            );

            return ~0U;
        }

        const LPWSTR fontRegistryPath = L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";

        HKEY hKey;
        LONG result;
        result = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, fontRegistryPath, 0, KEY_READ, &hKey );

        if( result != ERROR_SUCCESS )
        {
            _fontPath[0] = '\0';

            return ~0U;
        }

        DWORD maxValueNameSize;
        DWORD maxValueDataSize;
        result = ::RegQueryInfoKey( hKey, 0, 0, 0, 0, 0, 0, 0, &maxValueNameSize, &maxValueDataSize, 0, 0 );

        if( result != ERROR_SUCCESS )
        {
            _fontPath[0] = '\0';

            return ~0U;
        }

        DWORD valueIndex = 0;

        WChar unicode_fontPath[MENGINE_MAX_PATH] = { '\0' };

        do
        {
            DWORD valueDataSize = maxValueDataSize;
            DWORD valueNameSize = maxValueNameSize;

            DWORD valueType;
            WCHAR valueName[MAX_PATH];
            BYTE valueData[MAX_PATH * 2];

            result = ::RegEnumValue( hKey, valueIndex, valueName, &valueNameSize, 0, &valueType, valueData, &valueDataSize );

            valueIndex++;

            if( result != ERROR_SUCCESS || valueType != REG_SZ )
            {
                continue;
            }

            // Found a match
            if( ::_wcsicmp( unicode_fontName, valueName ) == 0 )
            {
                ::memcpy( unicode_fontPath, valueData, valueDataSize ); 

                break;
            }
        } while( result != ERROR_NO_MORE_ITEMS );

        ::RegCloseKey( hKey );

        if( ::wcslen( unicode_fontPath ) == 0 )
        {
            _fontPath[0] = '\0';

            return ~0U;
        }

        WCHAR winDir[MAX_PATH];
        ::GetWindowsDirectory( winDir, MAX_PATH );

        WCHAR fullDir[MAX_PATH];
        ::wsprintf( fullDir, L"%s\\Fonts\\%s"
            , winDir
            , unicode_fontPath
        );

        size_t utf8_size;
        if( Helper::unicodeToUtf8( fullDir, _fontPath, MENGINE_MAX_PATH, &utf8_size ) == false )
        {
            _fontPath[0] = '\0';

            return ~0U;
        }

        return utf8_size;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::getMaxClientResolution( Resolution & _resolution ) const
    {
        RECT workArea;
        ::SystemParametersInfo( SPI_GETWORKAREA, 0, &workArea, 0 );

        RECT clientArea = workArea;
        ::AdjustWindowRect( &clientArea, WS_OVERLAPPEDWINDOW, FALSE );
        uint32_t maxClientWidth = 2 * (workArea.right - workArea.left) - (clientArea.right - clientArea.left);
        uint32_t maxClientHeight = 2 * (workArea.bottom - workArea.top) - (clientArea.bottom - clientArea.top);

        _resolution.setWidth( maxClientWidth );
        _resolution.setHeight( maxClientHeight );
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & Win32Platform::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::hasPlatformTag( const ConstString & _tag ) const
    {
        return m_platformTags.hasTag( _tag );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::hasTouchpad() const
    {
        return m_touchpad;
    }
    //////////////////////////////////////////////////////////////////////////
    static LRESULT CALLBACK s_wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        switch( uMsg )
        {
            //case WM_NCCREATE:
        case WM_CREATE:
            {
                LPCREATESTRUCTW createStruct = (LPCREATESTRUCTW)lParam;

                Win32Platform * app = (Win32Platform *)createStruct->lpCreateParams;

#ifdef _WIN64
                ::SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)app );
#else
                ::SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG)app );
#endif

                return (LRESULT)NULL;
            }
            break;
        }

        LONG_PTR value = ::GetWindowLongPtr( hWnd, GWLP_USERDATA );

        Win32Platform * platform = (Win32Platform *)value;

        if( platform == NULL )
        {
            LRESULT result = ::DefWindowProc( hWnd, uMsg, wParam, lParam );

            return result;
        }

        LRESULT app_result = platform->wndProc( hWnd, uMsg, wParam, lParam );

        return app_result;
    }
    //////////////////////////////////////////////////////////////////////////
    LRESULT Win32Platform::wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        //LOGGER_WARNING( "wndProc [%x:%x] %x %x %x\n", m_hWnd, hWnd, uMsg, wParam, lParam );

        if( m_hWnd != hWnd )
        {
            LRESULT result = ::DefWindowProc( hWnd, uMsg, wParam, lParam );

            return result;
        }

        switch( uMsg )
        {
        case WM_ACTIVATE:
            {
                DWORD flagActive = LOWORD( wParam ); // флажок активизации
                BOOL minimized = (BOOL)HIWORD( wParam ); // флажок минимизации

                bool active = (flagActive != WA_INACTIVE) && (minimized == FALSE);

                this->setActive_( active );

                return FALSE;
            }break;
        case WM_ACTIVATEAPP:
            {
            }break;
        case WM_SETFOCUS:
            {
            }break;
        case WM_KILLFOCUS:
            {
            }break;
        case WM_PAINT:
            {
                if( m_update == false )
                {
                    bool fullscreen = APPLICATION_SERVICE()
                        ->getFullscreenMode();

                    if( fullscreen == false )
                    {
                        APPLICATION_SERVICE()
                            ->paint();
                    }
                }
            }break;
        case WM_DISPLAYCHANGE:
            {
                //DWORD width = LOWORD(lParam); // флажок активизации
                //DWORD height = HIWORD(lParam); // флажок минимизации

                //Resolution desktopResolution;
                //desktopResolution.setWidth( width );
                //desktopResolution.setHeight( height );

                //bool fullscreenMode = m_application->getFullscreenMode();

                //this->notifyWindowModeChanged( desktopResolution, fullscreenMode );
            }break;
        case WM_SIZE:
            {
                if( wParam == SIZE_MAXIMIZED )
                {
                    this->setActive_( true );
                }
                else if( wParam == SIZE_MINIMIZED )
                {
                    this->setActive_( false );
                }
                else if( wParam == SIZE_RESTORED /*&& m_application->getFullscreenMode() == true*/ )
                {
                    bool fullsreenMode = APPLICATION_SERVICE()
                        ->getFullscreenMode();

                    if( fullsreenMode == true )
                    {
                        this->setActive_( true );
                    }
                }
            }break;
        case WM_GETMINMAXINFO:
            {
                // Prevent the window from going smaller than some minimu size
                MINMAXINFO * info = (MINMAXINFO *)lParam;

                info->ptMinTrackSize.x = 100;
                info->ptMinTrackSize.y = 100;
            }break;
        case WM_CLOSE:
            {
                m_close = true;

                return FALSE;
            }break;
        case WM_SYSKEYDOWN:
            {
                uint32_t vkc = static_cast<uint32_t>(wParam);

                mt::vec2f point;
                this->calcCursorPosition_( point );

                EKeyCode code = (EKeyCode)vkc;

                Helper::pushKeyEvent( point.x, point.y, code, true, false );
            }break;
        case WM_SYSKEYUP:
            {
                uint32_t vkc = static_cast<uint32_t>(wParam);

                mt::vec2f point;
                this->calcCursorPosition_( point );

                EKeyCode code = (EKeyCode)vkc;

                Helper::pushKeyEvent( point.x, point.y, code, false, false );
            }break;
        case WM_SYSCOMMAND:
            {
                switch( wParam )
                {
                case SC_CLOSE:
                    {
                        m_close = true;
                    }break;

                case SC_KEYMENU:
                    {
                        if( lParam == 13 )
                        {
                            bool fullscreen = APPLICATION_SERVICE()
                                ->getFullscreenMode();

                            APPLICATION_SERVICE()
                                ->setFullscreenMode( !fullscreen );
                        }

                        //return FALSE;
                    }break;
                case SC_SCREENSAVE:
                    {
                        //Disable Screensave
                        return TRUE;
                    }break;
                case SC_MONITORPOWER:
                    {
                        return TRUE;
                    }break;
                }
            }break;
        case WM_SETCURSOR:
            {
                bool client = LOWORD( lParam ) == HTCLIENT;

                bool focus = APPLICATION_SERVICE()
                    ->isFocus();

                if( focus == true && client == true && m_cursorMode == false )
                {
                    ::SetCursor( NULL );
                }
                else
                {
                    if( m_cursor == NULL )
                    {
                        m_cursor = LoadCursor( NULL, IDC_ARROW );
                    }

                    ::SetCursor( m_cursor );
                }

                return FALSE;
            }break;
        case WM_DESTROY:
            m_close = true;

            return 0;
        }

        LRESULT input_result;
        if( this->wndProcInput( hWnd, uMsg, wParam, lParam, input_result ) == true )
        {
            return input_result;
        }

        LRESULT result = ::DefWindowProc( hWnd, uMsg, wParam, lParam );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_sonvertUTF32toUTF8( UINT32 _utf32, Char * _utf8 )
    {
        if( _utf32 <= 0x7F )
        {
            _utf8[0] = (Char)_utf32;
            _utf8[1] = '\0';
        }
        else if( _utf32 <= 0x7FF )
        {
            _utf8[0] = 0xC0 | (Char)((_utf32 >> 6) & 0x1F);
            _utf8[1] = 0x80 | (Char)(_utf32 & 0x3F);
            _utf8[2] = '\0';
        }
        else if( _utf32 <= 0xFFFF )
        {
            _utf8[0] = 0xE0 | (Char)((_utf32 >> 12) & 0x0F);
            _utf8[1] = 0x80 | (Char)((_utf32 >> 6) & 0x3F);
            _utf8[2] = 0x80 | (Char)(_utf32 & 0x3F);
            _utf8[3] = '\0';
        }
        else if( _utf32 <= 0x10FFFF )
        {
            _utf8[0] = 0xF0 | (Char)((_utf32 >> 18) & 0x0F);
            _utf8[1] = 0x80 | (Char)((_utf32 >> 12) & 0x3F);
            _utf8[2] = 0x80 | (Char)((_utf32 >> 6) & 0x3F);
            _utf8[3] = 0x80 | (Char)(_utf32 & 0x3F);
            _utf8[4] = '\0';
        }
        else
        {
            return false;
        }

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////
    //bool Win32Platform::wndProcTouch( HWND hWnd, WPARAM wParam, LPARAM lParam )
    //{
    //    MENGINE_UNUSED( hWnd );

    //    BOOL bHandled = FALSE;
    //    UINT cInputs = LOWORD( wParam );
    //    TOUCHINPUT pInputs[32];
    //    if( pInputs ) {
    //        if( GetTouchInputInfo( (HTOUCHINPUT)lParam, cInputs, pInputs, sizeof( TOUCHINPUT ) ) ) {
    //            for( UINT i = 0; i < cInputs; i++ ) {
    //                TOUCHINPUT ti = pInputs[i];
    //                //do something with each touch input entry
    //            }
    //            bHandled = TRUE;
    //        }
    //        else {
    //            /* handle the error here */
    //        }
    //    }
    //    else {
    //        /* handle the error here, probably out of memory */
    //    }
    //    if( bHandled ) {
    //        // if you handled the message, close the touch input handle and return
    //        CloseTouchInputHandle( (HTOUCHINPUT)lParam );
    //        return true;
    //    }

    //    return false;
    //}
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::wndProcInput( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT & _result )
    {
        MENGINE_UNUSED( hWnd );

        bool handle = false;

        switch( uMsg )
        {
        case WM_TIMER:
            {
                if( wParam == UTIMER_MOUSE_EVENT )
                {
                    m_mouseEvent.verify();
                }

                handle = true;
                _result = FALSE;
            }break;
        case UWM_MOUSE_LEAVE:
            {
                if( m_cursorInArea == true )
                {
                    m_cursorInArea = false;
                    ::InvalidateRect( hWnd, NULL, FALSE );
                    ::UpdateWindow( hWnd );

                    mt::vec2f point;
                    this->calcCursorPosition_( point );

                    Helper::pushMouseLeaveEvent( 0, point.x, point.y, 0.f );

                    if( (::GetKeyState( VK_LBUTTON ) & 0x8000) != 0 )
                    {
                        m_clickOutArea = true;
                    }
                }

                handle = true;
                _result = FALSE;
            }break;
            //case WM_TOUCH:
            //    {
            //        if( this->wndProcTouch( hWnd, wParam, lParam ) == true )
            //        {
            //            handle = true;
            //            _result = FALSE;
            //        }
            //    }break;
        case WM_MOUSEMOVE:
            {
                //::SetFocus( m_hWnd );

                mt::vec2f point;
                this->calcCursorPosition_( point );

                if( m_cursorInArea == false && m_fullscreen == false )
                {
                    m_cursorInArea = true;

                    m_mouseEvent.update();

                    ::InvalidateRect( hWnd, NULL, FALSE );
                    ::UpdateWindow( hWnd );

                    Helper::pushMouseEnterEvent( 0, point.x, point.y, 0.f );
                }

                if( m_clickOutArea == true )
                {
                    m_clickOutArea = false;

                    if( (::GetKeyState( VK_LBUTTON ) & 0x8000) == 0 )
                    {
                        Helper::pushMouseButtonEvent( 0, point.x, point.y, MC_LBUTTON, 0.f, false );
                    }
                }

                int32_t x = (int32_t)(int16_t)LOWORD( lParam );
                int32_t y = (int32_t)(int16_t)HIWORD( lParam );

                if( m_lastMouse == false )
                {
                    POINT p;
                    ::GetCursorPos( &p );
                    ::ScreenToClient( hWnd, &p );

                    m_lastMouseX = p.x;
                    m_lastMouseY = p.y;

                    m_lastMouse = true;
                }

                int32_t dx = x - m_lastMouseX;
                int32_t dy = y - m_lastMouseY;

                if( dx == 0 && dy == 0 )
                {
                    break;
                }

                m_lastMouseX = x;
                m_lastMouseY = y;

                float fdx = (float)dx;
                float fdy = (float)dy;

                RECT rect;
                if( ::GetClientRect( m_hWnd, &rect ) == FALSE )
                {
                    return false;
                }

                float width = static_cast<float>(rect.right - rect.left);
                float height = static_cast<float>(rect.bottom - rect.top);

                fdx /= width;
                fdy /= height;

                //printf( "%f %f %d %d\n"
                //	, point.x
                //	, point.y
                //	, int( point.x * 1024.f )
                //	, int( point.y * 768.f )
                //	);

                Helper::pushMouseMoveEvent( 0, point.x, point.y, fdx, fdy, 0.f );

                handle = true;
                _result = FALSE;
            }break;
        case WM_MOUSEWHEEL:
            {
                int32_t zDelta = (int32_t)(int16_t)(HIWORD( wParam ));

                mt::vec2f point;
                this->calcCursorPosition_( point );

                int32_t wheel = zDelta / WHEEL_DELTA;

                Helper::pushMouseWheelEvent( point.x, point.y, MC_LBUTTON, wheel );

                handle = true;
                _result = FALSE;
            }break;
        case WM_RBUTTONDBLCLK:
        case WM_LBUTTONDBLCLK:
            {
                m_isDoubleClick = true;

                handle = true;
                _result = FALSE;
            }
            break;
        case WM_LBUTTONDOWN:
            {
                mt::vec2f point;
                this->calcCursorPosition_( point );

                Helper::pushMouseButtonEvent( 0, point.x, point.y, MC_LBUTTON, 0.f, true );

                handle = true;
                _result = FALSE;
            }
            break;
        case WM_LBUTTONUP:
            {
                if( m_isDoubleClick == false )
                {
                    mt::vec2f point;
                    this->calcCursorPosition_( point );

                    Helper::pushMouseButtonEvent( 0, point.x, point.y, MC_LBUTTON, 0.f, false );
                }

                m_isDoubleClick = false;

                handle = true;
                _result = FALSE;
            }break;
        case WM_RBUTTONDOWN:
            {
                //::SetFocus(m_hWnd);

                mt::vec2f point;
                this->calcCursorPosition_( point );

                Helper::pushMouseButtonEvent( 0, point.x, point.y, MC_RBUTTON, 0.f, true );

                handle = true;
                _result = FALSE;
            }break;
        case WM_RBUTTONUP:
            {
                if( m_isDoubleClick == false )
                {
                    mt::vec2f point;
                    this->calcCursorPosition_( point );

                    Helper::pushMouseButtonEvent( 0, point.x, point.y, MC_RBUTTON, 0.f, false );
                }

                m_isDoubleClick = false;

                handle = true;
                _result = FALSE;
            }break;
        case WM_MBUTTONDOWN:
            {
                //::SetFocus(m_hWnd);

                mt::vec2f point;
                this->calcCursorPosition_( point );

                Helper::pushMouseButtonEvent( 0, point.x, point.y, MC_MBUTTON, 0.f, true );

                handle = true;
                _result = FALSE;
            }break;
        case WM_MBUTTONUP:
            {
                mt::vec2f point;
                this->calcCursorPosition_( point );

                Helper::pushMouseButtonEvent( 0, point.x, point.y, MC_MBUTTON, 0.f, false );

                handle = true;
                _result = FALSE;
            }break;
        case WM_KEYDOWN:
            {
                UINT vkc = static_cast<UINT>(wParam);

                mt::vec2f point;
                this->calcCursorPosition_( point );

                EKeyCode code = (EKeyCode)vkc;

                Helper::pushKeyEvent( point.x, point.y, code, true, false );

                handle = true;
                _result = FALSE;
            }break;
        case WM_KEYUP:
            {
                UINT vkc = static_cast<UINT>(wParam);

                mt::vec2f point;
                this->calcCursorPosition_( point );

                EKeyCode code = (EKeyCode)vkc;

                Helper::pushKeyEvent( point.x, point.y, code, false, false );

                handle = true;
                _result = FALSE;
            }break;
        case WM_UNICHAR:
            if( wParam == UNICODE_NOCHAR )
            {
                _result = TRUE;

            }break;
        case WM_CHAR:
            {
                Char utf8[5];
                if( s_sonvertUTF32toUTF8( (uint32_t)wParam, utf8 ) == true )
                {
                    mt::vec2f point;
                    this->calcCursorPosition_( point );

                    WChar text_code[2];
                    size_t text_code_size;
                    UNICODE_SYSTEM()
                        ->utf8ToUnicode( utf8, MENGINE_UNICODE_UNKNOWN_SIZE, text_code, 2, &text_code_size );

                    Helper::pushTextEvent( point.x, point.y, text_code[0] );

                    handle = true;
                    _result = FALSE;
                }
            }break;
        }

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    ATOM Win32Platform::registerClass_( WNDPROC _wndProc, int32_t _clsExtra, int32_t _wndExtra
        , HINSTANCE _hInstance, DWORD _hIcon, HBRUSH _hbrBackground
        , const WChar * _className )
    {
        WNDCLASS wc;
        ::ZeroMemory( &wc, sizeof( WNDCLASS ) );
        wc.cbClsExtra = _clsExtra;
        wc.cbWndExtra = _wndExtra;
        wc.style = /*CS_DBLCLKS |*/ CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = _wndProc;
        wc.hInstance = _hInstance;

        wc.hIcon = ::LoadIcon( _hInstance, MAKEINTRESOURCEW( _hIcon ) );
        wc.hCursor = ::LoadCursor( NULL, MAKEINTRESOURCEW( 32512 ) );

        wc.lpszClassName = _className;
        wc.hbrBackground = _hbrBackground;

        ATOM atom = ::RegisterClass( &wc );

        return atom;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::createWindow( const Resolution & _resolution, bool _fullscreen )
    {
        bool alreadyRunning = CONFIG_VALUE( "Game", "AlreadyRunning", true );

        if( alreadyRunning == true )
        {
            m_alreadyRunningMonitor = Helper::makeFactorableUnique<Win32AlreadyRunningMonitor>();

            if( m_alreadyRunningMonitor->initialize( EARP_SETFOCUS, MENGINE_WINDOW_CLASSNAME, m_projectTitle ) == false )
            {
                LOGGER_ERROR( "Application invalid running monitor"
                );

                return false;
            }
        }

        m_windowResolution = _resolution;

        HBRUSH black_brush = (HBRUSH)::GetStockObject( BLACK_BRUSH );

        // Register the window class		
        ATOM result = this->registerClass_(
            s_wndProc,
            0,
            0,
            m_hInstance,
            m_icon,
            black_brush,
            MENGINE_WINDOW_CLASSNAME
        );

        if( result == FALSE )
        {
            LOGGER_ERROR( "Can't register window class" );

            return false;
        }

        DWORD dwStyle = this->getWindowStyle_( _fullscreen );
        dwStyle &= ~WS_VISIBLE;

        RECT rc = this->getWindowsRect_( m_windowResolution, _fullscreen );

        DWORD exStyle = _fullscreen ? WS_EX_TOPMOST : 0;
        //DWORD exStyle = 0;

        HWND hWnd = ::CreateWindowEx( exStyle, MENGINE_WINDOW_CLASSNAME, m_projectTitle
            , dwStyle
            , rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top
            , NULL, NULL, m_hInstance, (LPVOID)this );

        if( hWnd == NULL )
        {
            return false;
        }

        m_hWnd = hWnd;

        //RegisterTouchWindow( m_hWnd, 0 );

        HWND hWndFgnd = ::GetForegroundWindow();

        if( hWndFgnd != m_hWnd )
        {
            LOGGER_MESSAGE( "Setup Foreground Window..." );

            ::ShowWindow( m_hWnd, SW_MINIMIZE );
            ::ShowWindow( m_hWnd, SW_RESTORE );

            //SetWindowPos(m_hWnd,hWndFgnd,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
            //SetWindowPos(hWndFgnd,m_hWnd,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
        }
        else
        {
            ::ShowWindow( m_hWnd, SW_SHOW );
        }

        if( _fullscreen == true )
        {
            Resolution desktopResolution;
            this->getDesktopResolution( desktopResolution );

            this->notifyWindowModeChanged( desktopResolution, true );
        }

        ::SetForegroundWindow( m_hWnd );          // Slightly Higher Priority
        ::SetFocus( m_hWnd );                     // Sets Keyboard Focus To The Window
        ::UpdateWindow( m_hWnd );

        m_mouseEvent.initialize( m_hWnd );

        const Char * option_fps = GET_OPTION_VALUE( "fps" );

        float activeFrameTimeDefault = 1000.f / 60.f;

        if( option_fps != nullptr )
        {
            float fps;
            Helper::stringalized( option_fps, &fps );

            activeFrameTimeDefault = 1000.f / fps;
        }

        m_activeFrameTime = CONFIG_VALUE2( "Engine", "MaxActiveFrameTime", activeFrameTimeDefault );

        bool vsync = APPLICATION_SERVICE()
            ->getVSync();

        bool maxfps = HAS_OPTION( "maxfps" );

        if( maxfps == false && vsync == false )
        {
            m_fpsMonitor = Helper::makeFactorableUnique<Win32FPSMonitor>();
            m_fpsMonitor->initialize();

            m_fpsMonitor->setActive( true );
            m_fpsMonitor->setFrameTime( m_activeFrameTime );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer Win32Platform::getWindowHandle() const
    {
        return m_hWnd;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
        LOGGER_MESSAGE( "resolution %d:%d fullscreen %d"
            , _resolution.getWidth()
            , _resolution.getHeight()
            , _fullscreen
        );

        m_windowResolution = _resolution;
        m_fullscreen = _fullscreen;
        m_cursorInArea = false;

        DWORD dwStyle = this->getWindowStyle_( m_fullscreen );

        RECT rc = this->getWindowsRect_( m_windowResolution, m_fullscreen );

        LONG dwExStyle = ::GetWindowLong( m_hWnd, GWL_EXSTYLE );

        if( m_fullscreen == false )
        {
            // When switching back to windowed mode, need to reset window size 
            // after device has been restored

            ::SetWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle & (~WS_EX_TOPMOST) );
            ::SetWindowLong( m_hWnd, GWL_STYLE, dwStyle );

            ::SetWindowPos(
                m_hWnd
                , HWND_NOTOPMOST
                , rc.left
                , rc.top
                , rc.right - rc.left
                , rc.bottom - rc.top
                , SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOACTIVATE
            );
        }
        else
        {
            ::SetWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle | WS_EX_TOPMOST );
            ::SetWindowLong( m_hWnd, GWL_STYLE, dwStyle );

            ::SetWindowPos(
                m_hWnd
                , HWND_TOPMOST
                , rc.left
                , rc.top
                , rc.right - rc.left
                , rc.bottom - rc.top
                //, SWP_FRAMECHANGED //| SWP_NOZORDER | SWP_NOACTIVATE  | SWP_FRAMECHANGED
                , SWP_NOACTIVATE
            );


            //::SetWindowPos(
            //    m_hWnd
            //    , 0
            //    , 0
            //    , 0
            //    , 0
            //    , 0
            //    , SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
            //    );
        }

        //::UpdateWindow( m_hWnd );
        //::ShowWindow( m_hWnd, SW_SHOW );
        //      ::SetFocus( m_hWnd );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::notifyVsyncChanged( bool _vsync )
    {
        bool novsync = HAS_OPTION( "novsync" );

        if( novsync == true )
        {
            if( m_fpsMonitor != nullptr )
            {
                m_fpsMonitor->setActive( true );
            }

            return;
        }

        m_vsync = _vsync;

        if( m_fpsMonitor != nullptr )
        {
            m_fpsMonitor->setActive( m_vsync == false );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::notifyCursorModeChanged( bool _mode )
    {
        m_cursorMode = _mode;

        if( m_cursor == NULL )
        {
            m_cursor = ::LoadCursor( NULL, IDC_ARROW );
        }

        m_cursor = m_cursorMode ? m_cursor : NULL;

        ::SetCursor( m_cursor );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::notifyCursorIconSetup( const ConstString & _name, const FilePath & _path, const MemoryInterfacePtr & _buffer )
    {
        MapCursors::iterator it_found = m_cursors.find( _path );

        if( it_found == m_cursors.end() )
        {
            if( _buffer->empty() == true )
            {
                LOGGER_ERROR( "'%s' buffer empty"
                    , _path.c_str()
                );

                return false;
            }

            PathString icoFile;
            icoFile += "IconCache";
            icoFile += '/';
            icoFile += _path;
            icoFile += ".ico";

            FilePath c_icoFile = Helper::stringizeFilePath( icoFile );

            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

            OutputStreamInterfacePtr stream = FILE_SERVICE()
                ->openOutputFile( fileGroup, c_icoFile, MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, false, "name '%s' path '%s' can't open output stream '%s'"
                , _name.c_str()
                , _path.c_str()
                , c_icoFile.c_str()
            );

            void * memory = _buffer->getBuffer();
            size_t size = _buffer->getSize();

            if( stream->write( memory, size ) == false )
            {
                LOGGER_ERROR( "name '%s' path '%s' can't write output stream '%s'"
                    , _name.c_str()
                    , _path.c_str()
                    , c_icoFile.c_str()
                );

                return false;
            }

            stream->flush();
            stream = nullptr;

            WString unicode_icoFile;
            if( Helper::utf8ToUnicode( c_icoFile, unicode_icoFile ) == false )
            {
                LOGGER_ERROR( "name '%s' path '%s' can't file name '%s' to unicode"
                    , _name.c_str()
                    , _path.c_str()
                    , c_icoFile.c_str()
                );

                return false;
            }

            HCURSOR cursor = ::LoadCursorFromFileW( unicode_icoFile.c_str() );

            if( cursor == NULL )
            {
                DWORD errCode = ::GetLastError();

                if( errCode != 0 )
                {
                    LOGGER_ERROR( "icon '%s' for file '%ls' errCode %d"
                        , _name.c_str()
                        , unicode_icoFile.c_str()
                        , errCode
                    );

                    return false;
                }
            }

            it_found = m_cursors.emplace( _name, cursor ).first;
        }

        m_cursor = it_found->second;

        ::SetCursor( m_cursor );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    DWORD Win32Platform::getWindowStyle_( bool _fullsreen ) const
    {
        DWORD dwStyle = WS_POPUP;

        if( _fullsreen == false )
        {
            dwStyle |= WS_CAPTION | WS_VISIBLE;

            //bool hasWindowPanel = true;
            //m_application->getHasWindowPanel();

            //if( hasWindowPanel == true )
            {
                dwStyle |= WS_SYSMENU | WS_MINIMIZEBOX;
            }

            //bool allowMaximize = m_application->getAllowFullscreenSwitchShortcut();

            //if( allowMaximize == true )
            //{
            //	dwStyle |= WS_MAXIMIZEBOX;
            //}
        }
        else
        {
            dwStyle |= WS_VISIBLE;
        }

        return dwStyle;
    }
    //////////////////////////////////////////////////////////////////////////
    RECT Win32Platform::getWindowsRect_( const Resolution & _resolution, bool _fullsreen ) const
    {
        RECT rc;
        ::SetRect( &rc, 0, 0, (int32_t)_resolution.getWidth(), (int32_t)_resolution.getHeight() );

        if( _fullsreen == false )
        {
            DWORD dwStyle = this->getWindowStyle_( _fullsreen );
            ::AdjustWindowRect( &rc, dwStyle, FALSE );

            RECT workArea;
            ::SystemParametersInfo( SPI_GETWORKAREA, 0, &workArea, 0 );

            LONG width = rc.right - rc.left;
            LONG height = rc.bottom - rc.top;

            rc.left = (workArea.left + workArea.right - width) / 2;
            rc.top = (workArea.top + workArea.bottom - height) / 2;

            rc.right = rc.left + width;
            rc.bottom = rc.top + height;
        }

        return rc;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::calcCursorPosition_( mt::vec2f & _point ) const
    {
        POINT cPos;
        if( ::GetCursorPos( &cPos ) == FALSE )
        {
            return false;
        }

        if( ::ScreenToClient( m_hWnd, &cPos ) == FALSE )
        {
            return false;
        }

        float x = static_cast<float>(cPos.x);
        float y = static_cast<float>(cPos.y);

        RECT rect;
        if( ::GetClientRect( m_hWnd, &rect ) == FALSE )
        {
            return false;
        }

        float width = static_cast<float>(rect.right - rect.left);
        float height = static_cast<float>(rect.bottom - rect.top);

        _point.x = x / width;
        _point.y = y / height;

        //_point.x = x;
        //_point.y = y;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::existDirectory( const Char * _path ) const
    {
        WChar unicode_path[MENGINE_MAX_PATH];
        if( Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar fullPath[MENGINE_MAX_PATH];
        Helper::pathCorrectBackslashToW( fullPath, unicode_path );

        Helper::pathRemoveFileSpecW( fullPath );

        size_t len = ::wcslen( fullPath );

        if( len == 0 )
        {
            return true;
        }

        if( fullPath[len - 1] == L':' )
        {
            return true;
        }

        DWORD attributes = GetFileAttributes( fullPath );

        if( attributes == INVALID_FILE_ATTRIBUTES )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::createDirectory( const Char * _path )
    {
        WChar unicode_path[MENGINE_MAX_PATH];
        if( Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        bool result = this->createDirectory_( unicode_path );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::createDirectory_( const WChar * _directoryPath )
    {
        size_t unicode_pathSize = ::wcslen( _directoryPath );

        WChar fullPath[MENGINE_MAX_PATH];

        if( unicode_pathSize != 0 )
        {
            Helper::pathCorrectBackslashToW( fullPath, _directoryPath );

            Helper::pathRemoveFileSpecW( fullPath );

            Helper::pathRemoveBackslashW( fullPath );

            if( ::PathIsDirectoryW( fullPath ) == FILE_ATTRIBUTE_DIRECTORY )
            {
                return true;
            }
        }
        else
        {
            fullPath[0] = L'.';
            fullPath[1] = L'\0';
        }

        VectorWString paths;

        for( ;; )
        {
            paths.emplace_back( fullPath );

            if( Helper::pathRemoveFileSpecW( fullPath ) == false )
            {
                break;
            }

            Helper::pathRemoveBackslashW( fullPath );

            if( ::PathIsDirectoryW( fullPath ) == FILE_ATTRIBUTE_DIRECTORY )
            {
                break;
            }
        }

        for( VectorWString::const_reverse_iterator
            it = paths.rbegin(),
            it_end = paths.rend();
            it != it_end;
            ++it )
        {
            const WString & path = *it;

            BOOL successful = CreateDirectory( path.c_str(), NULL );

            if( successful == FALSE )
            {
                DWORD err = GetLastError();

                switch( err )
                {
                case ERROR_ALREADY_EXISTS:
                    {
                        LOGGER_WARNING( "directory '%ls' alredy exists"
                            , path.c_str()
                        );

                        return false;
                    }break;
                case ERROR_PATH_NOT_FOUND:
                    {
                        LOGGER_WARNING( "directory '%ls' not found"
                            , path.c_str()
                        );

                        return false;
                    }break;
                default:
                    {
                        LOGGER_WARNING( "directory '%ls' unknown error %d"
                            , path.c_str()
                            , err
                        );

                        return false;
                    }break;
                }
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::existFile( const Char * _path )
    {
        WChar unicode_path[MENGINE_MAX_PATH];
        if( Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        bool result = this->existFile_( unicode_path );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::existFile_( const WChar * _path )
    {
        WChar pathCorrect[MENGINE_MAX_PATH];
        Helper::pathCorrectBackslashToW( pathCorrect, _path );

        size_t len = ::wcslen( pathCorrect );

        if( len == 0 )	// current dir
        {
            return true;
        }

        if( pathCorrect[len - 1] == L':' )	// root dir
        {
            return true;	// let it be
        }

        DWORD attributes = ::GetFileAttributes( pathCorrect );

        if( attributes == INVALID_FILE_ATTRIBUTES )
        {
            return false;
        }

        if( (attributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::removeFile( const Char * _path )
    {
        WChar unicode_path[MENGINE_MAX_PATH];
        if( Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        Char userPath[MENGINE_MAX_PATH];
        this->getUserPath( userPath );

        WChar unicode_userPath[MENGINE_MAX_PATH];
        if( Helper::utf8ToUnicode( userPath, unicode_userPath, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar pathCorrect[MENGINE_MAX_PATH];
        Helper::pathCorrectBackslashToW( pathCorrect, unicode_path );

        WChar fullPath[MENGINE_MAX_PATH];
        wcscpy( fullPath, unicode_userPath );
        wcscat( fullPath, pathCorrect );

        if( ::DeleteFile( fullPath ) == FALSE )
        {
            DWORD err = GetLastError();

            LOGGER_WARNING( "file '%ls' error '%d'"
                , fullPath
                , err
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_listDirectoryContents( const WChar * _dir, const WChar * _mask, const WChar * _path, const LambdaFilePath & _lambda, bool * _stop )
    {
        {
            WChar sPath[MENGINE_MAX_PATH] = { 0 };
            ::wcscpy( sPath, _dir );
            ::wcscat( sPath, _path );
            ::wcscat( sPath, _mask );

            WIN32_FIND_DATA fdFile;
            HANDLE hFind = ::FindFirstFileEx( sPath, FindExInfoStandard, &fdFile, FindExSearchNameMatch, NULL, 0 );

            if( hFind != INVALID_HANDLE_VALUE )
            {
                do
                {
                    if( ::wcscmp( fdFile.cFileName, L"." ) == 0
                        || ::wcscmp( fdFile.cFileName, L".." ) == 0 )
                    {
                        continue;
                    }

                    if( fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                    {
                        continue;
                    }

                    WChar sPath2[MENGINE_MAX_PATH] = { 0 };
                    ::wcscpy( sPath2, sPath );
                    ::wcscat( sPath2, L"\0" );
                    Helper::pathCorrectForwardslashW( sPath2 );

                    ::PathRemoveFileSpec( sPath2 );

                    WChar unicode_filepath[MENGINE_MAX_PATH] = { 0 };
                    ::PathCombine( unicode_filepath, sPath2, fdFile.cFileName );

                    WChar unicode_out[MENGINE_MAX_PATH] = { 0 };
                    if( wcslen( _dir ) != 0 )
                    {
                        ::PathRelativePathTo( unicode_out,
                            _dir,
                            FILE_ATTRIBUTE_DIRECTORY,
                            unicode_filepath,
                            FILE_ATTRIBUTE_NORMAL );
                    }
                    else
                    {
                        wcscpy( unicode_out, unicode_filepath );
                    }

                    Char utf8_filepath[MENGINE_MAX_PATH];
                    if( Helper::unicodeToUtf8( unicode_out, utf8_filepath, MENGINE_MAX_PATH ) == false )
                    {
                        ::FindClose( hFind );

                        return false;
                    }

                    FilePath fp = Helper::stringizeFilePath( utf8_filepath );

                    if( _lambda( fp ) == false )
                    {
                        ::FindClose( hFind );

                        *_stop = true;

                        return true;
                    }

                } while( ::FindNextFile( hFind, &fdFile ) != FALSE );
            }

            ::FindClose( hFind );
        }

        {
            WChar sPath[MENGINE_MAX_PATH] = { 0 };
            ::wcscpy( sPath, _dir );
            ::wcscat( sPath, _path );
            ::wcscat( sPath, L"*.*" );

            WIN32_FIND_DATA fdFile;
            HANDLE hFind = ::FindFirstFile( sPath, &fdFile );

            if( hFind == INVALID_HANDLE_VALUE )
            {
                return true;
            }

            do
            {
                if( ::wcscmp( fdFile.cFileName, L"." ) == 0
                    || ::wcscmp( fdFile.cFileName, L".." ) == 0 )
                {
                    continue;
                }

                if( (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
                {
                    continue;
                }

                //WChar nextPath[2048];
                //wsprintf( nextPath, L"%s%s\\", _path, fdFile.cFileName );
                WChar currentPath[MENGINE_MAX_PATH] = { 0 };
                ::wcscpy( currentPath, sPath );
                ::wcscat( currentPath, L"\0" );

                ::PathRemoveFileSpec( currentPath );

                WChar nextPath[MENGINE_MAX_PATH] = { 0 };
                ::PathCombine( nextPath, currentPath, fdFile.cFileName );

                ::wcscat( nextPath, L"\\" );

                bool stop;
                if( s_listDirectoryContents( _dir, _mask, nextPath, _lambda, &stop ) == false )
                {
                    ::FindClose( hFind );

                    return false;
                }

                if( stop == true )
                {
                    ::FindClose( hFind );

                    *_stop = true;

                    return true;
                }

            } while( ::FindNextFile( hFind, &fdFile ) != FALSE );

            ::FindClose( hFind );
        }

        *_stop = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const
    {
        WChar unicode_base[MENGINE_MAX_PATH];
        if( Helper::utf8ToUnicode( _base, unicode_base, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar unicode_mask[MENGINE_MAX_PATH];
        if( Helper::utf8ToUnicode( _mask, unicode_mask, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar unicode_path[MENGINE_MAX_PATH];
        if( Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar unicode_fullbase[MENGINE_MAX_PATH] = { 0 };
        ::GetFullPathName( unicode_base, MENGINE_MAX_PATH, unicode_fullbase, NULL );

        bool stop;
        if( s_listDirectoryContents( unicode_fullbase, unicode_mask, unicode_path, _lambda, &stop ) == false )
        {
            return false;
        }

        MENGINE_UNUSED( stop );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t Win32Platform::getFileTime( const Char * _path ) const
    {
        MENGINE_UNUSED( _path );

        return 0U;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::getDateTime( PlatformDateTime * _dateTime ) const
    {
        if( _dateTime == nullptr )
        {
            return;
        }

        SYSTEMTIME time;
        ::GetLocalTime( &time );

        _dateTime->year = time.wYear;
        _dateTime->month = time.wMonth;
        _dateTime->day = time.wDay;
        _dateTime->hour = time.wHour;
        _dateTime->minute = time.wMinute;
        _dateTime->second = time.wSecond;
        _dateTime->milliseconds = time.wMilliseconds;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::createDirectoryUser_( const WChar * _userPath, const WChar * _path, const WChar * _file, const void * _data, size_t _size )
    {
        WChar szPath[MENGINE_MAX_PATH] = { 0 };
        ::PathAppend( szPath, _userPath );

        WChar pathCorrect[MENGINE_MAX_PATH];
        Helper::pathCorrectBackslashToW( pathCorrect, _path );

        WChar fileCorrect[MENGINE_MAX_PATH];
        Helper::pathCorrectBackslashToW( fileCorrect, _file );

        ::PathAppend( szPath, pathCorrect );

        if( this->existFile_( szPath ) == false )
        {
            if( this->createDirectory_( szPath ) == false )
            {
                LOGGER_ERROR( "directory '%ls:%ls' invalid createDirectory '%ls'"
                    , pathCorrect
                    , fileCorrect
                    , szPath
                );

                return false;
            }
        }

        PathAppend( szPath, fileCorrect );

        HANDLE hFile = ::CreateFile( szPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

        if( hFile == INVALID_HANDLE_VALUE )
        {
            LOGGER_ERROR( "%ls:%ls invalid createFile %ls"
                , pathCorrect
                , fileCorrect
                , szPath
            );

            return false;
        }

        DWORD bytesWritten = 0;
        BOOL result = ::WriteFile( hFile, _data, (DWORD)_size, &bytesWritten, NULL );

        ::CloseHandle( hFile );

        if( result == FALSE )
        {
            LOGGER_ERROR( "%ls:%ls invalid writeFile %ls"
                , pathCorrect
                , fileCorrect
                , szPath
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::createDirectoryUserPicture( const Char * _path, const Char * _file, const void * _data, size_t _size )
    {
        WChar unicode_path[MENGINE_MAX_PATH];
        if( Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar unicode_file[MENGINE_MAX_PATH];
        if( Helper::utf8ToUnicode( _file, unicode_file, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WCHAR szPath[MENGINE_MAX_PATH];

        if( FAILED( ::SHGetFolderPath( NULL
            , CSIDL_COMMON_PICTURES | CSIDL_FLAG_CREATE
            , NULL
            , 0
            , szPath ) ) )
        {
            LOGGER_ERROR( "'%s:%s' invalid SHGetFolderPath CSIDL_COMMON_PICTURES"
                , _path
                , _file
            );

            return false;
        }

        if( this->createDirectoryUser_( szPath, unicode_path, unicode_file, _data, _size ) == false )
        {
            LOGGER_ERROR( "'%s:%s' invalid createDirectoryUser_ '%ls'"
                , _path
                , _file
                , szPath
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::createDirectoryUserMusic( const Char * _path, const Char * _file, const void * _data, size_t _size )
    {
        WChar unicode_path[MENGINE_MAX_PATH];
        if( Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar unicode_file[MENGINE_MAX_PATH];
        if( Helper::utf8ToUnicode( _file, unicode_file, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WCHAR szPath[MENGINE_MAX_PATH];

        if( FAILED( ::SHGetFolderPath( NULL
            , CSIDL_COMMON_MUSIC | CSIDL_FLAG_CREATE
            , NULL
            , 0
            , szPath ) ) )
        {
            LOGGER_ERROR( "'%s:%s' invalid SHGetFolderPath CSIDL_COMMON_MUSIC"
                , _path
                , _file
            );

            return false;
        }

        if( this->createDirectoryUser_( szPath, unicode_path, unicode_file, _data, _size ) == false )
        {
            LOGGER_ERROR( "'%s:%s' invalid createDirectoryUser_ '%ls'"
                , _path
                , _file
                , szPath
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::getErrorMessage( uint32_t _messageId, Char * _out, size_t _capacity ) const
    {
        LPTSTR errorText = NULL;

        if( ::FormatMessage(
            // use system message tables to retrieve error text
            FORMAT_MESSAGE_FROM_SYSTEM
            // allocate buffer on local heap for error text
            | FORMAT_MESSAGE_ALLOCATE_BUFFER
            // Important! will fail otherwise, since we're not 
            // (and CANNOT) pass insertion parameters
            | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
            _messageId,
            MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ),
            (LPTSTR)& errorText,  // output 
            0, // minimum size for output buffer
            NULL ) == 0 )
        {
            _out[0] = '\0';

            return false;
        }
        else
        {
            Helper::unicodeToUtf8( errorText, _out, _capacity );

            ::LocalFree( errorText );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::sleep( uint32_t _ms )
    {
        ::Sleep( _ms );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::getLocalMachineRegValue( const Char * _path, const Char * _key, Char * _value, size_t _size )
    {
        WChar unicode_path[MAX_PATH] = { '\0' };
        Helper::utf8ToUnicode( _path, unicode_path, MAX_PATH );

        WChar unicode_key[MAX_PATH] = { '\0' };
        Helper::utf8ToUnicode( _key, unicode_key, MAX_PATH );

        HKEY hKey;
        LONG lRes = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, unicode_path, 0, KEY_READ, &hKey );

        if( lRes == ERROR_FILE_NOT_FOUND )
        {
#ifdef _MSC_VER
            lRes = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, unicode_path, 0, KEY_READ | KEY_WOW64_64KEY, &hKey );
#endif
        }

        if( lRes != ERROR_SUCCESS )
        {
            LOGGER_ERROR( "RegOpenKeyEx HKEY_LOCAL_MACHINE '%s' get Error [%d]"
                , _path
                , lRes
            );

            return false;
        }

        WChar unicode_value[1024] = { L'\0' };

        DWORD dwBufferSize = 1024;
        LONG nError = ::RegQueryValueEx( hKey, unicode_key, 0, NULL, (LPBYTE)unicode_value, &dwBufferSize );

        ::RegCloseKey( hKey );

        if( nError != ERROR_SUCCESS )
        {
            LOGGER_ERROR( "RegQueryValueEx HKEY_LOCAL_MACHINE '%s' get Error [%d]"
                , _path
                , nError
            );

            return false;
        }

        Helper::unicodeToUtf8( unicode_value, _value, _size );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::createProcess( const Char * _process, const Char * _command, bool _wait, uint32_t * _exitCode )
    {
        if( _command == nullptr )
        {
            LOGGER_ERROR( "process '%s' invalid command line '%s'"
                , _process
                , _command
            );

            return false;
        }

        LOGGER_MESSAGE( "create process '%s %s'"
            , _process
            , _command
        );

        WChar unicode_process[MAX_PATH] = { '\0' };
        Helper::utf8ToUnicode( _process, unicode_process, MAX_PATH );

        WChar unicode_command[4096] = { '\0' };
        unicode_command[0] = ' ';
        Helper::utf8ToUnicode( _command, unicode_command + 1, 4095 );

        if( _wait == true )
        {
            SECURITY_ATTRIBUTES sa;
            sa.nLength = sizeof( sa );
            sa.lpSecurityDescriptor = NULL;
            sa.bInheritHandle = TRUE;

            WChar tempPathBuffer[MAX_PATH];
            ::GetTempPath( MAX_PATH, tempPathBuffer );

            WChar tempFileNameBuffer[MAX_PATH];
            ::GetTempFileName( tempPathBuffer,
                L"Process",
                0,
                tempFileNameBuffer );

            HANDLE hWriteTempFile = ::CreateFile( tempFileNameBuffer,
                FILE_APPEND_DATA,
                FILE_SHARE_WRITE | FILE_SHARE_READ,
                &sa,
                OPEN_ALWAYS,
                FILE_ATTRIBUTE_TEMPORARY,
                NULL );

            STARTUPINFO startupInfo = { 0 };
            startupInfo.cb = sizeof( STARTUPINFO );
            startupInfo.dwFlags = STARTF_USESTDHANDLES;
            startupInfo.hStdOutput = hWriteTempFile;
            startupInfo.hStdError = hWriteTempFile;
            startupInfo.hStdInput = NULL;

            PROCESS_INFORMATION processInfo = { 0 };
            if( ::CreateProcess( unicode_process, unicode_command
                , NULL
                , NULL
                , _wait == true ? TRUE : FALSE
                , NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL
                , NULL
                , &startupInfo
                , &processInfo ) == FALSE )
            {
                DWORD le = GetLastError();

                Char message[1024] = { '\0' };
                this->getErrorMessage( le, message, 1024 );

                LOGGER_ERROR( "CreateProcess '%s' return error: %s [%d]"
                    , _process
                    , message
                    , le
                );

                return false;
            }

            ::WaitForSingleObject( processInfo.hProcess, INFINITE );

            DWORD exitCode = 0;
            BOOL result = ::GetExitCodeProcess( processInfo.hProcess, &exitCode );

            ::CloseHandle( processInfo.hProcess );
            ::CloseHandle( processInfo.hThread );

            ::CloseHandle( hWriteTempFile );

            if( result == FALSE )
            {
                LOGGER_ERROR( "CreateProcess execute invalid get exit code\n[%s %s]\n"
                    , _process
                    , _command
                );

                return false;
            }

            LOGGER_MESSAGE( "result [%u]"
                , exitCode
            );

            if( exitCode != 0 )
            {
                HANDLE hReadTempFile = ::CreateFile( tempFileNameBuffer,
                    FILE_GENERIC_READ,
                    FILE_SHARE_WRITE | FILE_SHARE_READ,
                    &sa,
                    OPEN_ALWAYS,
                    FILE_ATTRIBUTE_TEMPORARY,
                    NULL );

                DWORD tempFileSizeHigh;
                DWORD tempFileSize = ::GetFileSize( hReadTempFile, &tempFileSizeHigh );

                Char tempFileBuffer[4096] = { 0 };

                DWORD dwBytesRead;
                DWORD nNumberOfBytesToRead = MENGINE_MIN( tempFileSize, 4096 );
                BOOL successful = ::ReadFile( hReadTempFile, tempFileBuffer, nNumberOfBytesToRead, &dwBytesRead, NULL );

                if( successful == TRUE )
                {
                    LOGGER_VERBOSE_LEVEL( Mengine::LM_ERROR, nullptr, 0 )("%s"
                        , tempFileBuffer
                        );
                }
                else
                {
                    LOGGER_VERBOSE_LEVEL( Mengine::LM_ERROR, nullptr, 0 )("invalid read file '%ls'"
                        , tempFileNameBuffer
                        );
                }

                CloseHandle( hReadTempFile );
            }

            if( _exitCode != nullptr )
            {
                *_exitCode = (uint32_t)exitCode;
            }
        }
        else
        {
            STARTUPINFO startupInfo = { 0 };
            startupInfo.cb = sizeof( STARTUPINFO );
            PROCESS_INFORMATION processInfo = { 0 };
            if( ::CreateProcess( unicode_process, unicode_command
                , NULL
                , NULL
                , FALSE
                , NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL
                , NULL
                , &startupInfo
                , &processInfo ) == FALSE )
            {
                DWORD le = GetLastError();

                Char message[1024] = { '\0' };
                this->getErrorMessage( le, message, 1024 );

                LOGGER_ERROR( "CreateProcess '%s' return error: %s [%d]"
                    , _process
                    , message
                    , le
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::abort()
    {
        ::abort();
    }
    //////////////////////////////////////////////////////////////////////////
    DynamicLibraryInterfacePtr Win32Platform::loadDynamicLibrary( const Char * _dynamicLibraryName )
    {
        LOGGER_INFO( "load dynamic library '%s'"
            , _dynamicLibraryName
        );

        Win32DynamicLibraryPtr dynamicLibrary = m_factoryDynamicLibraries
            ->createObject( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( dynamicLibrary, nullptr );

        dynamicLibrary->setName( _dynamicLibraryName );

        if( dynamicLibrary->load() == false )
        {
            LOGGER_ERROR( "can't load dynamic library '%s' [invalid load]"
                , _dynamicLibraryName
            );

            return nullptr;
        }

        return dynamicLibrary;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::getDesktopResolution( Resolution & _resolution ) const
    {
        int32_t cxscreen = ::GetSystemMetrics( SM_CXSCREEN );
        int32_t cyscreen = ::GetSystemMetrics( SM_CYSCREEN );

        _resolution.setWidth( cxscreen );
        _resolution.setHeight( cyscreen );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::getNoFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::getAlwaysFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32Platform::getCurrentPath( Char * _currentPath ) const
    {
        WChar currentPath[MENGINE_MAX_PATH];
        DWORD len = (DWORD)::GetCurrentDirectory( MENGINE_MAX_PATH, currentPath );

        if( len == 0 )
        {
            _currentPath[0] = '\0';

            return 0;
        }

        Helper::pathCorrectBackslashW( currentPath );

        currentPath[len] = L'/';
        currentPath[len + 1] = L'\0';

        size_t path_len;
        if( Helper::unicodeToUtf8( currentPath, _currentPath, MENGINE_MAX_PATH, &path_len ) == false )
        {
            _currentPath[0] = '\0';

            return 0;
        }

        return path_len;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32Platform::getUserPath( Char * _userPath ) const
    {
        bool developmentMode = HAS_OPTION( "dev" );
        bool roamingMode = HAS_OPTION( "roaming" );
        bool noroamingMode = HAS_OPTION( "noroaming" );

        if( developmentMode == true && roamingMode == false || noroamingMode == true )
        {
            WChar currentPath[MENGINE_MAX_PATH];
            DWORD len = (DWORD)::GetCurrentDirectory( MENGINE_MAX_PATH, currentPath );

            if( len == 0 )
            {
                LOGGER_ERROR( "failed to get current directory"
                );

                return 0;
            }

            ::PathRemoveBackslash( currentPath );

            Helper::pathCorrectBackslashW( currentPath );

            ::wcscat( currentPath, L"/User/" );

            size_t currentPathLen;
            if( Helper::unicodeToUtf8( currentPath, _userPath, MENGINE_MAX_PATH, &currentPathLen ) == false )
            {
                return 0;
            }

            return currentPathLen;
        }

        LPITEMIDLIST itemIDList;

        HRESULT hr = ::SHGetSpecialFolderLocation( NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, &itemIDList );

        if( hr != S_OK )
        {
            LPTSTR errorText = NULL;
            if( ::FormatMessage(
                // use system message tables to retrieve error text
                FORMAT_MESSAGE_FROM_SYSTEM
                // allocate buffer on local heap for error text
                | FORMAT_MESSAGE_ALLOCATE_BUFFER
                // Important! will fail otherwise, since we're not 
                // (and CANNOT) pass insertion parameters
                | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
                hr,
                MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
                (LPTSTR)& errorText,  // output 
                0, // minimum size for output buffer
                NULL ) == 0 )
            {
                LOGGER_ERROR( "SHGetSpecialFolderLocation invalid '%d'"
                    , hr
                );
            }
            else
            {
                LOGGER_ERROR( "SHGetSpecialFolderLocation invalid '%ls' '%d'"
                    , errorText
                    , hr
                );

                ::LocalFree( errorText );
            }

            return 0;
        }

        WChar currentPath[MENGINE_MAX_PATH];
        BOOL result = ::SHGetPathFromIDListW( itemIDList, currentPath );

        if( result == FALSE )
        {
            LOGGER_ERROR( "SHGetPathFromIDListW invalid"
            );

            return 0;
        }

        ::CoTaskMemFree( itemIDList );

        const Char * companyName = CONFIG_VALUE( "Project", "Company", "NONAME" );

        WChar companyNameW[MENGINE_APPLICATION_COMPANY_MAXNAME];
        if( Helper::utf8ToUnicode( companyName, companyNameW, MENGINE_APPLICATION_COMPANY_MAXNAME ) == false )
        {
            return 0;
        }

        WChar roamingPath[MENGINE_MAX_PATH];
        ::PathCombine( roamingPath, currentPath, companyNameW );

        const Char * projectName = CONFIG_VALUE( "Project", "Name", "UNKNOWN" );

        WChar projectNameW[MENGINE_APPLICATION_PROJECT_MAXNAME];
        if( Helper::utf8ToUnicode( projectName, projectNameW, MENGINE_APPLICATION_PROJECT_MAXNAME ) == false )
        {
            return 0;
        }

        ::PathCombine( roamingPath, roamingPath, projectNameW );

        ::PathRemoveBackslash( roamingPath );

        Helper::pathCorrectBackslashW( roamingPath );

        ::wcscat( roamingPath, L"/" );

        size_t currentPathLen;
        if( Helper::unicodeToUtf8( roamingPath, _userPath, MENGINE_MAX_PATH, &currentPathLen ) == false )
        {
            return 0;
        }

        return currentPathLen;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::minimizeWindow()
    {
        ::ShowWindow( m_hWnd, SW_MINIMIZE );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::setCursorPosition( const mt::vec2f & _pos )
    {
        POINT cPos = { (int32_t)_pos.x, (int32_t)_pos.y };
        ::ClientToScreen( m_hWnd, &cPos );

        ::SetCursorPos( cPos.x, cPos.y );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::showKeyboard()
    {
        //Empty - for iPad
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::hideKeyboard()
    {
        //Empty - for iPad
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::onEvent( const ConstString & _event, const MapWParams & _params )
    {
        MENGINE_UNUSED( _event );
        MENGINE_UNUSED( _params );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    float Win32Platform::getJoystickAxis( uint32_t _index ) const
    {
        MENGINE_UNUSED( _index );

        //FixMe

        return 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::openUrlInDefaultBrowser( const Char * _url )
    {
        WChar unicode_url[4096];
        if( Helper::utf8ToUnicode( _url, unicode_url, 4096 ) == false )
        {
            return false;
        }

        ::ShellExecute( NULL, L"open", unicode_url, NULL, NULL, SW_SHOWNORMAL );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::setActive_( bool _active )
    {
        if( m_active == _active )
        {
            return;
        }

        m_active = _active;

        bool nopause = APPLICATION_SERVICE()
            ->getNopause();

        if( m_fpsMonitor != nullptr )
        {
            if( m_active == true || nopause == true )
            {
                m_fpsMonitor->setFrameTime( m_activeFrameTime );
            }
            else
            {
                m_fpsMonitor->setFrameTime( m_inactiveFrameTime );
            }
        }

        mt::vec2f point;
        if( this->calcCursorPosition_( point ) == true )
        {
            Helper::pushMousePositionEvent( 0, point.x, point.y, 0.f );
        }

        if( m_active == false )
        {
            Helper::pushMouseLeaveEvent( 0, point.x, point.y, 0.f );
        }
        else
        {
            Helper::pushMouseEnterEvent( 0, point.x, point.y, 0.f );
        }

        if( nopause == false )
        {
            APPLICATION_SERVICE()
                ->setFocus( m_active );

            INPUT_SERVICE()
                ->onFocus( m_active );

            bool turnSound = m_active;
            APPLICATION_SERVICE()
                ->turnSound( turnSound );
        }
        else
        {
            APPLICATION_SERVICE()
                ->setFocus( true );

            INPUT_SERVICE()
                ->onFocus( true );

            APPLICATION_SERVICE()
                ->turnSound( true );
        }

        if( m_active == true )
        {
            if( m_cursor == NULL )
            {
                m_cursor = LoadCursor( NULL, IDC_ARROW );
            }

            ::SetCursor( m_cursor );
        }
        else
        {
            ::SetCursor( NULL );
        }
    }
}
