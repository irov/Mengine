#include "Win32Platform.h"

#include "Interface/OptionsServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/TimeSystemInterface.h"
#include "Interface/EnumeratorServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "Win32DynamicLibrary.h"
#include "Win32DateTimeProvider.h"
#include "Win32AntifreezeMonitor.h"
#include "Win32CriticalErrorsMonitor.h"

#include "Kernel/UnicodeHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/Stringalized.h"
#include "Kernel/StringHelper.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Stringstream.h"
#include "Config/StdString.h"

#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(push, 0) 
#endif

#include "Environment/Windows/DbgHelp.h"

#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(pop) 
#endif

#include <clocale>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <cerrno>

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_DEVELOPMENT_USER_FOLDER_NAME
#define MENGINE_DEVELOPMENT_USER_FOLDER_NAME L"User"
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_WINDOW_CLASSNAME
#define MENGINE_WINDOW_CLASSNAME L"MengineWindow"
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Platform, Mengine::Win32Platform );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32Platform::Win32Platform()
        : m_hInstance( NULL )
        , m_hWnd( NULL )
        , m_performanceSupport( false )
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
        , m_clickOutAreaLeftButton( false )
        , m_clickOutAreaRightButton( false )
        , m_isDoubleClick( false )
        , m_cursorMode( false )
        , m_cursor( nullptr )
        , m_lastMouse( false )
        , m_lastMouseX( 0 )
        , m_lastMouseY( 0 )
        , m_touchpad( false )
        , m_desktop( false )
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
        m_hInstance = GetModuleHandle( NULL );

        if( QueryPerformanceFrequency( &m_performanceFrequency ) == TRUE )
        {
            m_performanceSupport = true;
        }

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
        m_desktop = true;

        if( HAS_OPTION( "win32" ) )
        {
            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN32" ) );

            m_touchpad = false;
            m_desktop = true;
        }
        else if( HAS_OPTION( "win64" ) )
        {
            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN64" ) );

            m_touchpad = false;
            m_desktop = true;
        }
        else if( HAS_OPTION( "mac" ) )
        {
            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "MAC" ) );

            m_touchpad = false;
            m_desktop = true;
        }
        else if( HAS_OPTION( "ios" ) )
        {
            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "IOS" ) );

#ifdef _WIN32
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN32" ) );
#endif

            m_touchpad = true;
            m_desktop = false;
        }
        else if( HAS_OPTION( "android" ) )
        {
            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "ANDROID" ) );

            m_touchpad = true;
            m_desktop = false;
        }
        else if( HAS_OPTION( "wp" ) )
        {
            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WP" ) );

            m_touchpad = true;
            m_desktop = false;
        }

        const Char * option_platform = GET_OPTION_VALUE( "platform", nullptr );

        if( option_platform != nullptr )
        {
            Char uppercase_option_platform[256];
            Helper::toupper( option_platform, uppercase_option_platform, 256 );

            m_platformTags.clear();
            m_platformTags.addTag( Helper::stringizeString( option_platform ) );
        }

        if( HAS_OPTION( "touchpad" ) )
        {
            m_touchpad = true;
        }

        if( HAS_OPTION( "desktop" ) )
        {
            m_desktop = true;
        }

        bool developmentMode = HAS_OPTION( "dev" );

        if( developmentMode == true )
        {
            m_antifreezeMonitor = Helper::makeFactorableUnique<Win32AntifreezeMonitor>( MENGINE_DOCUMENT_FACTORABLE );
        }

        m_factoryDynamicLibraries = Helper::makeFactoryPool<Win32DynamicLibrary, 8>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryDateTimeProviders = Helper::makeFactoryPool<Win32DateTimeProvider, 8>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::_finalizeService()
    {
        m_active = false;

        m_platformTags.clear();

        m_cursors.clear();

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
                LOGGER_ERROR( "invalid UnregisterClass '%ls'"
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
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDateTimeProviders );

        m_factoryDynamicLibraries = nullptr;
        m_factoryDateTimeProviders = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::_runService()
    {
#if defined(MENGINE_DEBUG)
        {
            bool developmentMode = HAS_OPTION( "dev" );

            bool roamingMode = HAS_OPTION( "roaming" );
            bool noroamingMode = HAS_OPTION( "noroaming" );

            if( developmentMode == true && (roamingMode == false || noroamingMode == true) )
            {
                Char userPath[MENGINE_MAX_PATH] = {'\0'};
                this->getUserPath( userPath );

                String dumpPath;
                dumpPath += userPath;
                dumpPath += "Dump";
                dumpPath += "_";

                DateTimeProviderInterfacePtr dateTimeProvider =
                    this->createDateTimeProvider( MENGINE_DOCUMENT_FACTORABLE );

                PlatformDateTime dateTime;
                dateTimeProvider->getLocalDateTime( &dateTime );

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

        DateTimeProviderInterfacePtr dateTimeProvider =
            this->createDateTimeProvider( MENGINE_DOCUMENT_FACTORABLE );

        PlatformDateTime dateTime;
        dateTimeProvider->getLocalDateTime( &dateTime );

        LOGGER_MESSAGE( "Date: %02u.%02u.%u, %02u:%02u:%02u"
            , dateTime.day
            , dateTime.month
            , dateTime.year
            , dateTime.hour
            , dateTime.minute
            , dateTime.second
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
            LOGGER_ERROR( "Application not setup Process DPI Aware" );
        }
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
    bool Win32Platform::isDebuggerPresent() const
    {
        if( ::IsDebuggerPresent() == FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::createProcessDump( const Char * _dumpPath, void * const _pExceptionPointers, bool _full )
    {
        MENGINE_UNUSED( _dumpPath );
        MENGINE_UNUSED( _pExceptionPointers );
        MENGINE_UNUSED( _full );

#if defined(MENGINE_DEBUG)
        if( ::IsDebuggerPresent() == TRUE )
        {
            return false;
        }

        WString unicode_processDumpPath;
        Helper::utf8ToUnicode( _dumpPath, &unicode_processDumpPath );

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
    uint32_t Win32Platform::addTimer( float _milliseconds, const LambdaTimer & _lambda, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        uint32_t new_id = GENERATE_UNIQUE_IDENTITY();

        TimerDesc desc;
        desc.id = new_id;
        desc.milliseconds = _milliseconds;
        desc.time = _milliseconds;
        desc.lambda = _lambda;

#ifdef MENGINE_DEBUG
        desc.doc = _doc;
#endif

        m_timers.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::removeTimer( uint32_t _id )
    {
        VectorTimers::iterator it_found = std::find_if( m_timers.begin(), m_timers.end(), [_id]( const TimerDesc & _desc )
        {
            return _desc.id == _id;
        } );

        MENGINE_ASSERTION_FATAL( it_found != m_timers.end() );

        TimerDesc & desc = *it_found;

        desc.id = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t Win32Platform::getTicks() const
    {
        if( m_performanceSupport == false )
        {
            return 0ULL;
        }

        LARGE_INTEGER performanceCount;
        ::QueryPerformanceCounter( &performanceCount );

        LONGLONG ticks = performanceCount.QuadPart / m_performanceFrequency.QuadPart;

        return (uint64_t)ticks;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::runPlatform()
    {
        if( m_antifreezeMonitor != nullptr )
        {
            if( CONFIG_VALUE( "Platform", "AntifreezeMonitor", true ) == true )
            {
                if( m_antifreezeMonitor->initialize() == false )
                {
                    return false;
                }
            }
            else
            {
                m_antifreezeMonitor = nullptr;
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

        {
            while( m_close == false )
            {
                uint64_t currentTime = TIME_SYSTEM()
                    ->getTimeMilliseconds();

                float frameTime = (float)(currentTime - m_prevTime);

                m_prevTime = currentTime;

                if( m_antifreezeMonitor != nullptr )
                {
                    m_antifreezeMonitor->ping();
                }

                MSG  msg;
                while( ::PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != FALSE )
                {
                    ::TranslateMessage( &msg );
                    ::DispatchMessage( &msg );
                }

                for( TimerDesc & desc : m_timers )
                {
                    if( desc.id == 0 )
                    {
                        continue;
                    }

                    desc.time -= frameTime;

                    if( desc.time > 0.f )
                    {
                        continue;
                    }

                    desc.time += desc.milliseconds;

                    desc.lambda();
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

                if( (focus == true && m_active == true) || nopause == true )
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

        if( m_antifreezeMonitor != nullptr )
        {
            m_antifreezeMonitor->finalize();
            m_antifreezeMonitor = nullptr;
        }

        if( m_alreadyRunningMonitor != nullptr )
        {
            m_alreadyRunningMonitor->finalize();
            m_alreadyRunningMonitor = nullptr;
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
        if( _projectTitle == nullptr )
        {
            m_projectTitle[0] = '\0';

            return;
        }

        Helper::utf8ToUnicodeSize( _projectTitle, MENGINE_UNKNOWN_SIZE, m_projectTitle, MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32Platform::getProjectTitle( Char * const _projectTitle ) const
    {
        size_t utf8Size;
        Helper::unicodeToUtf8( m_projectTitle, _projectTitle, MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME, &utf8Size );

        return utf8Size;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32Platform::getShortPathName( const Char * _path, Char * const _shortpath ) const
    {
        WChar unicode_path[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR( "invalid convert path to unicode" );

            return 0;
        }

        WChar unicode_shortpath[MENGINE_MAX_PATH] = {L'\0'};
        DWORD len = ::GetShortPathName( unicode_path, unicode_shortpath, MENGINE_MAX_PATH );

        if( Helper::unicodeToUtf8Size( unicode_shortpath, (size_t)len, _shortpath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR( "invalid convert shortpath to utf8" );

            return 0;
        }

        return (size_t)len;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32Platform::getSystemFontPath( const Char * _fontName, Char * const _fontPath ) const
    {
        WChar unicode_fontName[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _fontName, unicode_fontName, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR( "invalid convert fontName '%s' to unicode"
                , _fontName
            );

            return MENGINE_PATH_INVALID_LENGTH;
        }

        const WChar * fontRegistryPath = L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";

        HKEY hKey;
        LONG result;
        result = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, fontRegistryPath, 0, KEY_READ, &hKey );

        if( result != ERROR_SUCCESS )
        {
            _fontPath[0] = '\0';

            return MENGINE_PATH_INVALID_LENGTH;
        }

        DWORD maxValueNameSize;
        DWORD maxValueDataSize;
        result = ::RegQueryInfoKey( hKey, 0, 0, 0, 0, 0, 0, 0, &maxValueNameSize, &maxValueDataSize, 0, 0 );

        if( result != ERROR_SUCCESS )
        {
            _fontPath[0] = '\0';

            return MENGINE_PATH_INVALID_LENGTH;
        }

        DWORD valueIndex = 0;

        WChar unicode_fontPath[MENGINE_MAX_PATH] = {L'\0'};

        do
        {
            DWORD valueDataSize = maxValueDataSize;
            DWORD valueNameSize = maxValueNameSize;

            DWORD valueType;
            WCHAR valueName[MENGINE_MAX_PATH] = {L'\0'};
            BYTE valueData[MENGINE_MAX_PATH * 2] = {0};

            result = ::RegEnumValue( hKey, valueIndex, valueName, &valueNameSize, 0, &valueType, valueData, &valueDataSize );

            valueIndex++;

            if( result != ERROR_SUCCESS || valueType != REG_SZ )
            {
                continue;
            }

            // Found a match
            if( ::_wcsicmp( unicode_fontName, valueName ) == 0 )
            {
                MENGINE_MEMCPY( unicode_fontPath, valueData, valueDataSize );

                break;
            }
        } while( result != ERROR_NO_MORE_ITEMS );

        ::RegCloseKey( hKey );

        if( MENGINE_WCSLEN( unicode_fontPath ) == 0 )
        {
            _fontPath[0] = '\0';

            return MENGINE_PATH_INVALID_LENGTH;
        }

        WCHAR winDir[MENGINE_MAX_PATH] = {L'\0'};
        ::GetWindowsDirectory( winDir, MENGINE_MAX_PATH );

        WCHAR fullDir[MENGINE_MAX_PATH] = {L'\0'};
        ::wsprintf( fullDir, L"%s\\Fonts\\%s"
            , winDir
            , unicode_fontPath
        );

        size_t utf8_size;
        if( Helper::unicodeToUtf8( fullDir, _fontPath, MENGINE_MAX_PATH, &utf8_size ) == false )
        {
            _fontPath[0] = '\0';

            return MENGINE_PATH_INVALID_LENGTH;
        }

        return utf8_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::getMaxClientResolution( Resolution * const _resolution ) const
    {
        RECT workArea;
        if( ::SystemParametersInfo( SPI_GETWORKAREA, 0, &workArea, 0 ) == FALSE )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "invalid get system parameters info [error: %lu]"
                , error
            );

            return false;
        }

        DWORD dwStyle = this->getWindowStyle_( false );
        DWORD dwExStyle = this->getWindowExStyle_( false );

        RECT clientArea = workArea;
        if( ::AdjustWindowRectEx( &clientArea, dwStyle, FALSE, dwExStyle ) == FALSE )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "invalid adjust window rect [error: %lu]"
                , error
            );

            return false;
        }

        uint32_t maxClientWidth = 2 * (workArea.right - workArea.left) - (clientArea.right - clientArea.left);
        uint32_t maxClientHeight = 2 * (workArea.bottom - workArea.top) - (clientArea.bottom - clientArea.top);

        _resolution->setWidth( maxClientWidth );
        _resolution->setHeight( maxClientHeight );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & Win32Platform::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::hasPlatformTag( const ConstString & _tag ) const
    {
        bool result = m_platformTags.hasTag( _tag );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::isDesktop() const
    {
        return m_desktop;
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

        for( const Win32ProcessDesc & desc : m_win32ProcessHandlers )
        {
            desc.lambda( hWnd, uMsg, wParam, lParam );
        }

        switch( uMsg )
        {
        case WM_ACTIVATE:
            {
                DWORD flagActive = LOWORD( wParam ); // флажок активизации
                BOOL minimized = (BOOL)HIWORD( wParam ); // флажок минимизации

                bool active = (flagActive != WA_INACTIVE) && (minimized == FALSE);

                this->setActive_( active );

                return 0;
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

                return 0;
            }break;
        case WM_SYSKEYDOWN:
            {
                uint32_t vkc = static_cast<uint32_t>(wParam);

                mt::vec2f point;
                if( this->calcCursorPosition_( &point ) == false )
                {
                    return 0;
                }

                EKeyCode code = (EKeyCode)vkc;

                Helper::pushKeyEvent( point.x, point.y, code, true, false );
            }break;
        case WM_SYSKEYUP:
            {
                uint32_t vkc = static_cast<uint32_t>(wParam);

                mt::vec2f point;
                if( this->calcCursorPosition_( &point ) == false )
                {
                    return 0;
                }

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
                        else
                        {
                            return 0;
                        }
                    }break;
                case SC_SCREENSAVE:
                    {
                        //Disable Screensave
                        return 1;
                    }break;
                case SC_MONITORPOWER:
                    {
                        return 1;
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
                        m_cursor = ::LoadCursor( NULL, IDC_ARROW );
                    }

                    ::SetCursor( m_cursor );
                }

                return 0;
            }break;
        case WM_DESTROY:
            m_close = true;

            return 0;
        }

        LRESULT input_result;
        if( this->wndProcInput( hWnd, uMsg, wParam, lParam, &input_result ) == true )
        {
            return input_result;
        }

        LRESULT result = ::DefWindowProc( hWnd, uMsg, wParam, lParam );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_sonvertUTF32toUTF8( UINT32 _utf32, Char * const _utf8 )
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
    bool Win32Platform::wndProcInput( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT * const _result )
    {
        bool handle = false;

        switch( uMsg )
        {
        case WM_TIMER:
            {
                if( wParam == MENGINE_UTIMER_MOUSE_EVENT )
                {
                    m_mouseEvent.verify();
                }

                handle = true;
                *_result = 0;
            }break;
        case UWM_MOUSE_LEAVE:
            {
                if( m_cursorInArea == true )
                {
                    m_cursorInArea = false;
                    ::InvalidateRect( hWnd, NULL, FALSE );
                    ::UpdateWindow( hWnd );

                    mt::vec2f point;
                    if( this->calcCursorPosition_( &point ) == false )
                    {
                        return 0;
                    }

                    Helper::pushMouseLeaveEvent( 0, point.x, point.y, 0.f );

                    if( (::GetKeyState( VK_LBUTTON ) & 0x8000) != 0 )
                    {
                        m_clickOutAreaLeftButton = true;
                    }

                    if( (::GetKeyState( VK_RBUTTON ) & 0x8000) != 0 )
                    {
                        m_clickOutAreaRightButton = true;
                    }
                }

                handle = true;
                *_result = 0;
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
                if( this->calcCursorPosition_( &point ) == false )
                {
                    return false;
                }

                if( m_clickOutAreaLeftButton == true )
                {
                    m_clickOutAreaLeftButton = false;

                    if( (::GetKeyState( VK_LBUTTON ) & 0x8000) == 0 )
                    {
                        Helper::pushMouseButtonEvent( 0, point.x, point.y, MC_LBUTTON, 0.f, false );
                    }
                }

                if( m_clickOutAreaRightButton == true )
                {
                    m_clickOutAreaRightButton = false;

                    if( (::GetKeyState( VK_RBUTTON ) & 0x8000) == 0 )
                    {
                        Helper::pushMouseButtonEvent( 0, point.x, point.y, MC_RBUTTON, 0.f, false );
                    }
                }

                if( m_cursorInArea == false && m_fullscreen == false )
                {
                    m_cursorInArea = true;

                    m_mouseEvent.update();

                    ::InvalidateRect( hWnd, NULL, FALSE );
                    ::UpdateWindow( hWnd );

                    Helper::pushMouseEnterEvent( 0, point.x, point.y, 0.f );
                }

                int32_t x = (int32_t)(int16_t)LOWORD( lParam );
                int32_t y = (int32_t)(int16_t)HIWORD( lParam );

                if( m_lastMouse == false )
                {
                    POINT p;
                    if( ::GetCursorPos( &p ) == FALSE )
                    {
                        DWORD error = ::GetLastError();

                        LOGGER_ERROR( "invalid get cursor pos [error: %lu]"
                            , error
                        );

                        return false;
                    }

                    if( ::ScreenToClient( hWnd, &p ) == FALSE )
                    {
                        DWORD error = ::GetLastError();

                        LOGGER_ERROR( "invalid screen to client [error: %lu]"
                            , error
                        );

                        return false;
                    }

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
                    DWORD error = ::GetLastError();

                    LOGGER_ERROR( "invalid get client rect [error: %lu]"
                        , error
                    );

                    return false;
                }

                float width = static_cast<float>(rect.right - rect.left);
                float height = static_cast<float>(rect.bottom - rect.top);

                fdx /= width;
                fdy /= height;

                Helper::pushMouseMoveEvent( 0, point.x, point.y, fdx, fdy, 0.f );

                handle = true;
                *_result = 0;
            }break;
        case WM_MOUSEWHEEL:
            {
                int32_t zDelta = (int32_t)(int16_t)(HIWORD( wParam ));

                mt::vec2f point;
                if( this->calcCursorPosition_( &point ) == false )
                {
                    return 0;
                }

                int32_t wheel = zDelta / WHEEL_DELTA;

                Helper::pushMouseWheelEvent( point.x, point.y, MC_LBUTTON, wheel );

                handle = true;
                *_result = 0;
            }break;
        case WM_RBUTTONDBLCLK:
        case WM_LBUTTONDBLCLK:
            {
                m_isDoubleClick = true;

                handle = true;
                *_result = 0;
            }
            break;
        case WM_LBUTTONDOWN:
            {
                mt::vec2f point;
                if( this->calcCursorPosition_( &point ) == false )
                {
                    return 0;
                }

                Helper::pushMouseButtonEvent( 0, point.x, point.y, MC_LBUTTON, 0.f, true );

                handle = true;
                *_result = 0;
            }
            break;
        case WM_LBUTTONUP:
            {
                if( m_isDoubleClick == false )
                {
                    mt::vec2f point;
                    if( this->calcCursorPosition_( &point ) == false )
                    {
                        return 0;
                    }

                    Helper::pushMouseButtonEvent( 0, point.x, point.y, MC_LBUTTON, 0.f, false );
                }

                m_isDoubleClick = false;

                handle = true;
                *_result = 0;
            }break;
        case WM_RBUTTONDOWN:
            {
                mt::vec2f point;
                if( this->calcCursorPosition_( &point ) == false )
                {
                    return 0;
                }

                Helper::pushMouseButtonEvent( 0, point.x, point.y, MC_RBUTTON, 0.f, true );

                handle = true;
                *_result = FALSE;
            }break;
        case WM_RBUTTONUP:
            {
                if( m_isDoubleClick == false )
                {
                    mt::vec2f point;
                    if( this->calcCursorPosition_( &point ) == false )
                    {
                        return 0;
                    }

                    Helper::pushMouseButtonEvent( 0, point.x, point.y, MC_RBUTTON, 0.f, false );
                }

                m_isDoubleClick = false;

                handle = true;
                *_result = 0;
            }break;
        case WM_MBUTTONDOWN:
            {
                mt::vec2f point;
                if( this->calcCursorPosition_( &point ) == false )
                {
                    return 0;
                }

                Helper::pushMouseButtonEvent( 0, point.x, point.y, MC_MBUTTON, 0.f, true );

                handle = true;
                *_result = 0;
            }break;
        case WM_MBUTTONUP:
            {
                mt::vec2f point;
                if( this->calcCursorPosition_( &point ) == false )
                {
                    return 0;
                }

                Helper::pushMouseButtonEvent( 0, point.x, point.y, MC_MBUTTON, 0.f, false );

                handle = true;
                *_result = 0;
            }break;
        case WM_KEYDOWN:
            {
                UINT vkc = static_cast<UINT>(wParam);

                mt::vec2f point;
                if( this->calcCursorPosition_( &point ) == false )
                {
                    return 0;
                }

                EKeyCode code = (EKeyCode)vkc;

                Helper::pushKeyEvent( point.x, point.y, code, true, false );

                handle = true;
                *_result = 0;
            }break;
        case WM_KEYUP:
            {
                UINT vkc = static_cast<UINT>(wParam);

                mt::vec2f point;
                if( this->calcCursorPosition_( &point ) == false )
                {
                    return 0;
                }

                EKeyCode code = (EKeyCode)vkc;

                Helper::pushKeyEvent( point.x, point.y, code, false, false );

                handle = true;
                *_result = FALSE;
            }break;
        case WM_UNICHAR:
            if( wParam == UNICODE_NOCHAR )
            {
                *_result = 1;

            }break;
        case WM_CHAR:
            {
                Char utf8[5];
                if( s_sonvertUTF32toUTF8( (uint32_t)wParam, utf8 ) == true )
                {
                    mt::vec2f point;
                    if( this->calcCursorPosition_( &point ) == false )
                    {
                        return 0;
                    }

                    WChar text_code[2];
                    Helper::utf8ToUnicode( utf8, text_code, 2 );

                    Helper::pushTextEvent( point.x, point.y, text_code[0] );

                    handle = true;
                    *_result = 0;
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
        m_windowResolution = _resolution;
        m_fullscreen = _fullscreen;

        bool Platform_AlreadyRunning = CONFIG_VALUE( "Platform", "AlreadyRunning", true );

        if( Platform_AlreadyRunning == true )
        {
            m_alreadyRunningMonitor = Helper::makeFactorableUnique<Win32AlreadyRunningMonitor>( MENGINE_DOCUMENT_FACTORABLE );

            if( m_alreadyRunningMonitor->initialize( EARP_SETFOCUS, MENGINE_WINDOW_CLASSNAME, m_projectTitle ) == false )
            {
                LOGGER_ERROR( "Application invalid running monitor"
                );

                return false;
            }
        }

        const Char * option_fps = GET_OPTION_VALUE( "fps", nullptr );

        float activeFrameTimeDefault = 1000.f / 60.f;

        if( option_fps != nullptr )
        {
            float fps;
            if( Helper::stringalized( option_fps, &fps ) == true )
            {
                activeFrameTimeDefault = 1000.f / fps;
            }
        }

        m_activeFrameTime = CONFIG_VALUE2( "Engine", "MaxActiveFrameTime", activeFrameTimeDefault );

        bool vsync = APPLICATION_SERVICE()
            ->getVSync();

        bool maxfps = HAS_OPTION( "maxfps" );

        if( maxfps == false && vsync == false )
        {
            m_fpsMonitor = Helper::makeFactorableUnique<Win32FPSMonitor>( MENGINE_DOCUMENT_FACTORABLE );
            m_fpsMonitor->initialize();

            m_fpsMonitor->setActive( true );
            m_fpsMonitor->setFrameTime( m_activeFrameTime );
        }

        bool Platform_WithoutWindow = CONFIG_VALUE( "Platform", "WithoutWindow", false );

        if( Platform_WithoutWindow == true || HAS_OPTION( "norender" ) == true )
        {
            LOGGER_MESSAGE( "platform without window" );

            return true;
        }

        HBRUSH black_brush = (HBRUSH)::GetStockObject( BLACK_BRUSH );

        // Register the window class
        ATOM result = this->registerClass_( &s_wndProc
            , 0
            , 0
            , m_hInstance
            , m_icon
            , black_brush
            , MENGINE_WINDOW_CLASSNAME );

        if( result == 0 )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "Can't register window class [error: %lu]"
                , error
            );

            return false;
        }

        DWORD dwStyle = this->getWindowStyle_( _fullscreen );
        dwStyle &= ~WS_VISIBLE;

        RECT rc;
        if( this->calcWindowsRect_( m_windowResolution, _fullscreen, &rc ) == false )
        {
            return false;
        }

        DWORD dwExStyle = this->getWindowExStyle_( _fullscreen );

        HWND hWnd = ::CreateWindowEx( dwExStyle, MENGINE_WINDOW_CLASSNAME, m_projectTitle
            , dwStyle
            , rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top
            , NULL, NULL, m_hInstance, (LPVOID)this );

        if( hWnd == NULL )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "Can't create window [error: %lu]"
                , error
            );

            return false;
        }

        m_hWnd = hWnd;

        HWND hWndFgnd = ::GetForegroundWindow();

        if( hWndFgnd != m_hWnd )
        {
            LOGGER_MESSAGE( "Setup Foreground Window..." );

            ::ShowWindow( m_hWnd, SW_MINIMIZE );
            ::ShowWindow( m_hWnd, SW_RESTORE );
        }
        else
        {
            ::ShowWindow( m_hWnd, SW_SHOW );
        }

        if( _fullscreen == true )
        {
            Resolution desktopResolution;
            this->getDesktopResolution( &desktopResolution );

            this->notifyWindowModeChanged( desktopResolution, true );
        }
        else
        {
            this->notifyWindowModeChanged( m_windowResolution, false );
        }

        ::SetForegroundWindow( m_hWnd );
        ::SetFocus( m_hWnd );
        ::UpdateWindow( m_hWnd );

        m_mouseEvent.initialize( m_hWnd );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    HWND Win32Platform::getWindowHandle() const
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
        DWORD dwFullscreenExStyle = this->getWindowExStyle_( true );

        RECT rc;
        if( this->calcWindowsRect_( m_windowResolution, m_fullscreen, &rc ) == false )
        {
            return;
        }

        LONG dwExStyle = ::GetWindowLong( m_hWnd, GWL_EXSTYLE );

        if( m_fullscreen == false )
        {
            ::SetWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle & (~dwFullscreenExStyle) );
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
            ::SetWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle | dwFullscreenExStyle );
            ::SetWindowLong( m_hWnd, GWL_STYLE, dwStyle );

            ::SetWindowPos(
                m_hWnd
                , HWND_TOPMOST
                , rc.left
                , rc.top
                , rc.right - rc.left
                , rc.bottom - rc.top
                , SWP_NOACTIVATE
            );
        }
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
    bool Win32Platform::notifyCursorIconSetup( const ConstString & _name, const ContentInterface * _content, const MemoryInterfacePtr & _buffer )
    {
        const FilePath & filePath = _content->getFilePath();

        MapCursors::iterator it_found = m_cursors.find( filePath );

        if( it_found == m_cursors.end() )
        {
            if( _buffer->empty() == true )
            {
                LOGGER_ERROR( "'%s' buffer empty"
                    , filePath.c_str()
                );

                return false;
            }

            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

            PathString icoFile;
            icoFile += ".icon_cache/";
            icoFile += filePath;
            icoFile += ".ico";

            FilePath c_icoFile = Helper::stringizeFilePath( icoFile );

            if( fileGroup->createDirectory( c_icoFile ) == false )
            {
                LOGGER_ERROR( "invalid create directory '.icon_cache'" );

                return false;
            }

            OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( fileGroup, c_icoFile, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, "name '%s' path '%s' can't open output stream '%s'"
                , _name.c_str()
                , filePath.c_str()
                , c_icoFile.c_str()
            );

            void * memory = _buffer->getBuffer();
            size_t size = _buffer->getSize();

            if( stream->write( memory, size ) == false )
            {
                LOGGER_ERROR( "name '%s' path '%s' can't write output stream '%s'"
                    , _name.c_str()
                    , filePath.c_str()
                    , c_icoFile.c_str()
                );

                return false;
            }

            stream->flush();
            stream = nullptr;

            PathString icoFullFile;
            icoFullFile += fileGroup->getFolderPath();
            icoFullFile += icoFile;

            WString unicode_icoFullFile;
            if( Helper::utf8ToUnicode( icoFullFile, &unicode_icoFullFile ) == false )
            {
                LOGGER_ERROR( "name '%s' path '%s' can't file name '%s' to unicode"
                    , _name.c_str()
                    , filePath.c_str()
                    , c_icoFile.c_str()
                );

                return false;
            }

            const WChar * unicode_icoFile_str = unicode_icoFullFile.c_str();

            HCURSOR cursor = ::LoadCursorFromFileW( unicode_icoFile_str );

            if( cursor == NULL )
            {
                DWORD errCode = ::GetLastError();

                if( errCode != 0 )
                {
                    LOGGER_ERROR( "icon '%s' for file '%ls' errCode %lu"
                        , _name.c_str()
                        , unicode_icoFullFile.c_str()
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
            //  dwStyle |= WS_MAXIMIZEBOX;
            //}
        }
        else
        {
            dwStyle |= WS_POPUP;
            dwStyle |= WS_VISIBLE;
        }

        return dwStyle;
    }
    //////////////////////////////////////////////////////////////////////////
    DWORD Win32Platform::getWindowExStyle_( bool _fullsreen ) const
    {
        DWORD dwExStyle = WS_EX_APPWINDOW;

        if( _fullsreen == false )
        {
            dwExStyle |= WS_EX_WINDOWEDGE;
        }

        return dwExStyle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::calcWindowsRect_( const Resolution & _resolution, bool _fullsreen, RECT * const _rect ) const
    {
        RECT rc;
        if( ::SetRect( &rc, 0, 0, (int32_t)_resolution.getWidth(), (int32_t)_resolution.getHeight() ) == FALSE )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "invalid set rect [error: %lu]"
                , error
            );

            return false;
        }

        if( _fullsreen == false )
        {
            DWORD dwStyle = this->getWindowStyle_( _fullsreen );
            DWORD dwStyleEx = this->getWindowExStyle_( _fullsreen );

            if( ::AdjustWindowRectEx( &rc, dwStyle, FALSE, dwStyleEx ) == FALSE )
            {
                DWORD error = ::GetLastError();

                LOGGER_ERROR( "invalid adjust window rect [error: %lu]"
                    , error
                );

                return false;
            }

            RECT workArea;
            if( ::SystemParametersInfo( SPI_GETWORKAREA, 0, &workArea, 0 ) == FALSE )
            {
                DWORD error = ::GetLastError();

                LOGGER_ERROR( "invalid system parameters info [error: %lu]"
                    , error
                );

                return false;
            }

            LONG width = rc.right - rc.left;
            LONG height = rc.bottom - rc.top;

            LONG left = (workArea.left + workArea.right - width) / 2;
            LONG top = (workArea.top + workArea.bottom - height) / 2;

            if( top > 0 )
            {
                rc.top = (workArea.top + workArea.bottom - height) / 2;
            }
            else
            {
                rc.top = 0;
            }

            if( left > 0 )
            {
                rc.left = (workArea.left + workArea.right - width) / 2;
            }
            else
            {
                rc.left = 0;
            }

            rc.right = rc.left + width;
            rc.bottom = rc.top + height;
        }

        *_rect = rc;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::calcCursorPosition_( mt::vec2f * const _point ) const
    {
        POINT cPos;
        if( ::GetCursorPos( &cPos ) == FALSE )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "invalid get cursor pos [error: %lu]"
                , error
            );

            return false;
        }

        if( ::ScreenToClient( m_hWnd, &cPos ) == FALSE )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "invalid screen to client [error: %lu]"
                , error
            );

            return false;
        }

        float x = static_cast<float>(cPos.x);
        float y = static_cast<float>(cPos.y);

        RECT rect;
        if( ::GetClientRect( m_hWnd, &rect ) == FALSE )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "invalid get client rect [error: %lu]"
                , error
            );

            return false;
        }

        float width = static_cast<float>(rect.right - rect.left);
        float height = static_cast<float>(rect.bottom - rect.top);

        _point->x = x / width;
        _point->y = y / height;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::existDirectory( const Char * _directoryPath ) const
    {
        MENGINE_ASSERTION_FATAL( MENGINE_STRLEN( _directoryPath ) > 0 && (MENGINE_STRRCHR( _directoryPath, '.' ) > MENGINE_STRRCHR( _directoryPath, MENGINE_PATH_DELIM ) || _directoryPath[MENGINE_STRLEN( _directoryPath ) - 1] == '/') );

        WChar unicode_path[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _directoryPath, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar fullPath[MENGINE_MAX_PATH] = {L'\0'};
        Helper::pathCorrectBackslashToW( fullPath, unicode_path );

        Helper::pathRemoveFileSpecW( fullPath );

        size_t len = MENGINE_WCSLEN( fullPath );

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
    bool Win32Platform::createDirectory( const Char * _directoryPath )
    {
        MENGINE_ASSERTION_FATAL( MENGINE_STRLEN( _directoryPath ) > 0 && (MENGINE_STRRCHR( _directoryPath, '.' ) > MENGINE_STRRCHR( _directoryPath, MENGINE_PATH_DELIM ) || _directoryPath[MENGINE_STRLEN( _directoryPath ) - 1] == '/') );

        WChar unicode_path[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _directoryPath, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        bool result = this->createDirectory_( unicode_path );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::createDirectory_( const WChar * _directoryPath )
    {
        size_t unicode_pathSize = MENGINE_WCSLEN( _directoryPath );

        WChar fullPath[MENGINE_MAX_PATH] = {L'\0'};

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

            BOOL successful = ::CreateDirectory( path.c_str(), NULL );

            if( successful == FALSE )
            {
                DWORD err = ::GetLastError();

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
                        LOGGER_WARNING( "directory '%ls' unknown error: %lu"
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
    bool Win32Platform::existFile( const Char * _filePath )
    {
        WChar unicode_path[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _filePath, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        bool result = this->existFile_( unicode_path );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::existFile_( const WChar * _filePath )
    {
        WChar pathCorrect[MENGINE_MAX_PATH] = {L'\0'};
        Helper::pathCorrectBackslashToW( pathCorrect, _filePath );

        size_t len = MENGINE_WCSLEN( pathCorrect );

        if( len == 0 )
        {
            return true;
        }

        if( pathCorrect[len - 1] == L':' )
        {
            return true;
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
    bool Win32Platform::removeFile( const Char * _filePath )
    {
        WChar unicode_path[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _filePath, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar pathCorrect[MENGINE_MAX_PATH] = {L'\0'};
        Helper::pathCorrectBackslashToW( pathCorrect, unicode_path );

        if( ::DeleteFile( pathCorrect ) == FALSE )
        {
            DWORD err = ::GetLastError();

            MENGINE_UNUSED( err );

            LOGGER_WARNING( "file '%ls' error '%lu'"
                , pathCorrect
                , err
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::moveFile( const Char * _oldFilePath, const Char * _newFilePath )
    {
        WChar unicode_oldFilePath[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _oldFilePath, unicode_oldFilePath, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar oldFilePathCorrect[MENGINE_MAX_PATH] = {L'\0'};
        Helper::pathCorrectBackslashToW( oldFilePathCorrect, unicode_oldFilePath );

        WChar unicode_newFilePath[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _newFilePath, unicode_newFilePath, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar newFilePathCorrect[MENGINE_MAX_PATH] = {L'\0'};
        Helper::pathCorrectBackslashToW( newFilePathCorrect, unicode_newFilePath );

#ifdef MENGINE_DEBUG
        DWORD oldFileAttributes = ::GetFileAttributes( oldFilePathCorrect );

        if( oldFileAttributes != INVALID_FILE_ATTRIBUTES && (oldFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )
        {
            LOGGER_WARNING( "invalid move old file '%ls' it's directory"
                , newFilePathCorrect
            );

            return false;
        }
#endif

        DWORD newFileAttributes = ::GetFileAttributes( newFilePathCorrect );

        if( newFileAttributes != INVALID_FILE_ATTRIBUTES )
        {
#ifdef MENGINE_DEBUG
            if( (newFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )
            {
                LOGGER_WARNING( "invalid move file '%ls' it's directory"
                    , newFilePathCorrect
                );

                return false;
            }
#endif

            if( ::DeleteFile( newFilePathCorrect ) == FALSE )
            {
                DWORD err = ::GetLastError();

                MENGINE_UNUSED( err );

                LOGGER_WARNING( "invalid move file '%ls' error '%lu'"
                    , newFilePathCorrect
                    , err
                );
            }
        }

        if( ::MoveFile( oldFilePathCorrect, newFilePathCorrect ) == FALSE )
        {
            DWORD error = ::GetLastError();

            Char str_le[1024] = {'\0'};
            this->getLastErrorMessage( &error, str_le, 1024 );

            LOGGER_WARNING( "file '%ls' move to '%ls' error: %s [%lu]"
                , oldFilePathCorrect
                , newFilePathCorrect
                , str_le
                , error
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool listDirectoryContents( const WChar * _dir, const WChar * _mask, const WChar * _path, const LambdaFilePath & _lambda, bool * const _stop )
        {
            {
                WChar sPath[MENGINE_MAX_PATH] = {L'\0'};
                MENGINE_WCSCPY( sPath, _dir );
                MENGINE_WCSCAT( sPath, _path );
                MENGINE_WCSCAT( sPath, _mask );

                WIN32_FIND_DATA fdFile;
                HANDLE hFind = ::FindFirstFileEx( sPath, FindExInfoStandard, &fdFile, FindExSearchNameMatch, NULL, 0 );

                if( hFind != INVALID_HANDLE_VALUE )
                {
                    do
                    {
                        if( MENGINE_WCSCMP( fdFile.cFileName, L"." ) == 0 ||
                            MENGINE_WCSCMP( fdFile.cFileName, L".." ) == 0 )
                        {
                            continue;
                        }

                        if( fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                        {
                            continue;
                        }

                        WChar sPath2[MENGINE_MAX_PATH] = {L'\0'};
                        MENGINE_WCSCPY( sPath2, sPath );
                        MENGINE_WCSCAT( sPath2, L"\0" );

                        Helper::pathCorrectForwardslashW( sPath2 );

                        ::PathRemoveFileSpec( sPath2 );

                        WChar unicode_filepath[MENGINE_MAX_PATH] = {L'\0'};
                        ::PathCombine( unicode_filepath, sPath2, fdFile.cFileName );

                        WChar unicode_out[MENGINE_MAX_PATH] = {L'\0'};
                        if( MENGINE_WCSLEN( _dir ) != 0 )
                        {
                            ::PathRelativePathTo( unicode_out,
                                _dir,
                                FILE_ATTRIBUTE_DIRECTORY,
                                unicode_filepath,
                                FILE_ATTRIBUTE_NORMAL );
                        }
                        else
                        {
                            MENGINE_WCSCPY( unicode_out, unicode_filepath );
                        }

                        Char utf8_filepath[MENGINE_MAX_PATH] = {'\0'};
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
                WChar sPath[MENGINE_MAX_PATH] = {L'\0'};
                MENGINE_WCSCPY( sPath, _dir );
                MENGINE_WCSCAT( sPath, _path );
                MENGINE_WCSCAT( sPath, L"*.*" );

                WIN32_FIND_DATA fdFile;
                HANDLE hFind = ::FindFirstFile( sPath, &fdFile );

                if( hFind == INVALID_HANDLE_VALUE )
                {
                    return true;
                }

                do
                {
                    if( MENGINE_WCSCMP( fdFile.cFileName, L"." ) == 0
                        || MENGINE_WCSCMP( fdFile.cFileName, L".." ) == 0 )
                    {
                        continue;
                    }

                    if( (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
                    {
                        continue;
                    }

                    WChar currentPath[MENGINE_MAX_PATH] = {L'\0'};
                    MENGINE_WCSCPY( currentPath, sPath );
                    MENGINE_WCSCAT( currentPath, L"\0" );

                    ::PathRemoveFileSpec( currentPath );

                    WChar nextPath[MENGINE_MAX_PATH] = {L'\0'};
                    ::PathCombine( nextPath, currentPath, fdFile.cFileName );

                    MENGINE_WCSCAT( nextPath, L"\\" );

                    bool stop;
                    if( Detail::listDirectoryContents( _dir, _mask, nextPath, _lambda, &stop ) == false )
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
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const
    {
        WChar unicode_base[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _base, unicode_base, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar unicode_mask[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _mask, unicode_mask, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar unicode_path[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar unicode_fullbase[MENGINE_MAX_PATH] = {L'\0'};
        ::GetFullPathName( unicode_base, MENGINE_MAX_PATH, unicode_fullbase, NULL );

        bool stop;
        if( Detail::listDirectoryContents( unicode_fullbase, unicode_mask, unicode_path, _lambda, &stop ) == false )
        {
            return false;
        }

        MENGINE_UNUSED( stop );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t Win32Platform::getFileTime( const Char * _filePath ) const
    {
        WChar unicode_filePath[MENGINE_MAX_PATH] = {L'\0'};
        Helper::utf8ToUnicode( _filePath, unicode_filePath, MENGINE_MAX_PATH );

        HANDLE handle = ::CreateFile( unicode_filePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

        FILETIME creation;
        FILETIME access;
        FILETIME write;

        if( ::GetFileTime( handle, &creation, &access, &write ) == FALSE )
        {
            ::CloseHandle( handle );

            return 0U;
        }

        ::CloseHandle( handle );

        time_t time = this->getFileUnixTime( &write );

        return time;
    }
    //////////////////////////////////////////////////////////////////////////
    DateTimeProviderInterfacePtr Win32Platform::createDateTimeProvider( const DocumentPtr & _doc )
    {
        Win32DateTimeProviderPtr dateTimeProvider = m_factoryDateTimeProviders->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( dateTimeProvider );

        return dateTimeProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::createDirectoryUser_( const WChar * _userPath, const WChar * _path, const WChar * _file, const void * _data, size_t _size )
    {
        WChar szPath[MENGINE_MAX_PATH] = {L'\0'};
        ::PathAppend( szPath, _userPath );

        WChar pathCorrect[MENGINE_MAX_PATH] = {L'\0'};
        Helper::pathCorrectBackslashToW( pathCorrect, _path );

        WChar fileCorrect[MENGINE_MAX_PATH] = {L'\0'};
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

        ::PathAppend( szPath, fileCorrect );

        HANDLE hFile = ::CreateFile( szPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

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
    bool Win32Platform::updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath )
    {
        WChar unicode_directoryPath[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _directoryPath, unicode_directoryPath, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar unicode_filePath[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _filePath, unicode_filePath, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WCHAR szPath[MENGINE_MAX_PATH] = {L'\0'};
        if( FAILED( ::SHGetFolderPath( NULL
            , CSIDL_COMMON_PICTURES | CSIDL_FLAG_CREATE
            , NULL
            , 0
            , szPath ) ) )
        {
            LOGGER_ERROR( "'%s:%s' invalid SHGetFolderPath CSIDL_COMMON_PICTURES"
                , _directoryPath
                , _filePath
            );

            return false;
        }

        WChar unicode_directoryPath_correct[MENGINE_MAX_PATH] = {L'\0'};
        Helper::pathCorrectBackslashToW( unicode_directoryPath_correct, unicode_directoryPath );

        ::PathAppend( szPath, unicode_directoryPath_correct );

        WChar unicode_filePath_correct[MENGINE_MAX_PATH] = {L'\0'};
        Helper::pathCorrectBackslashToW( unicode_filePath_correct, unicode_filePath );

        ::PathAppend( szPath, unicode_filePath_correct );

        if( this->existFile_( szPath ) == false )
        {
            return false;
        }

        if( ::SystemParametersInfo( SPI_SETDESKWALLPAPER, 0, szPath, SPIF_UPDATEINIFILE ) == FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        WChar unicode_path[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _directoryPath, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar unicode_file[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _filePath, unicode_file, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WCHAR szPath[MENGINE_MAX_PATH] = {L'\0'};
        if( FAILED( ::SHGetFolderPath( NULL
            , CSIDL_COMMON_PICTURES | CSIDL_FLAG_CREATE
            , NULL
            , 0
            , szPath ) ) )
        {
            LOGGER_ERROR( "'%s:%s' invalid SHGetFolderPath CSIDL_COMMON_PICTURES"
                , _directoryPath
                , _filePath
            );

            return false;
        }

        if( this->createDirectoryUser_( szPath, unicode_path, unicode_file, _data, _size ) == false )
        {
            LOGGER_ERROR( "'%s:%s' invalid createDirectoryUser_ '%ls'"
                , _directoryPath
                , _filePath
                , szPath
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        WChar unicode_path[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _directoryPath, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar unicode_file[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _filePath, unicode_file, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WCHAR szPath[MENGINE_MAX_PATH] = {L'\0'};
        if( FAILED( ::SHGetFolderPath( NULL
            , CSIDL_COMMON_MUSIC | CSIDL_FLAG_CREATE
            , NULL
            , 0
            , szPath ) ) )
        {
            LOGGER_ERROR( "'%s:%s' invalid SHGetFolderPath CSIDL_COMMON_MUSIC"
                , _directoryPath
                , _filePath
            );

            return false;
        }

        if( this->createDirectoryUser_( szPath, unicode_path, unicode_file, _data, _size ) == false )
        {
            LOGGER_ERROR( "'%s:%s' invalid createDirectoryUser_ '%ls'"
                , _directoryPath
                , _filePath
                , szPath
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::getErrorMessage( DWORD _messageId, Char * const _out, size_t _capacity ) const
    {
        LPTSTR errorText = NULL;

        if( ::FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS
            , NULL
            , _messageId
            , MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US )
            , (LPTSTR)&errorText
            , 0
            , NULL ) != 0 )
        {
            bool successful = Helper::unicodeToUtf8( errorText, _out, _capacity );

            ::LocalFree( errorText );

            if( successful == false )
            {
                return false;
            }

            return true;
        }

        _out[0] = '\0';

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::getLastErrorMessage( DWORD * const _error, Char * const _out, size_t _capacity ) const
    {
        DWORD error = ::GetLastError();

        *_error = error;

        bool result = this->getErrorMessage( error, _out, _capacity );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::sleep( uint32_t _ms )
    {
        ::Sleep( _ms );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::getLocalMachineRegValue( const Char * _path, const Char * _key, Char * const _value, size_t _size )
    {
        WChar unicode_path[MENGINE_MAX_PATH] = {'\0'};
        Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH );

        WChar unicode_key[MENGINE_MAX_PATH] = {'\0'};
        Helper::utf8ToUnicode( _key, unicode_key, MENGINE_MAX_PATH );

        HKEY hKey;
        LSTATUS lRes = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, unicode_path, 0, KEY_READ, &hKey );

        if( lRes == ERROR_FILE_NOT_FOUND )
        {
#ifdef _MSC_VER
            lRes = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, unicode_path, 0, KEY_READ | KEY_WOW64_64KEY, &hKey );
#endif
        }

        if( lRes != ERROR_SUCCESS )
        {
            LOGGER_ERROR( "RegOpenKeyEx HKEY_LOCAL_MACHINE '%s' get Error [%ld]"
                , _path
                , lRes
            );

            return false;
        }

        WChar unicode_value[1024] = {L'\0'};
        DWORD dwBufferSize = 1024;
        LSTATUS nError = ::RegQueryValueEx( hKey, unicode_key, 0, NULL, (LPBYTE)unicode_value, &dwBufferSize );

        ::RegCloseKey( hKey );

        if( nError != ERROR_SUCCESS )
        {
            LOGGER_ERROR( "RegQueryValueEx HKEY_LOCAL_MACHINE '%s' get Error [%ld]"
                , _path
                , nError
            );

            return false;
        }

        Helper::unicodeToUtf8( unicode_value, _value, _size );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::createProcess( const Char * _process, const Char * _command, bool _wait, uint32_t * const _exitCode )
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

        WChar unicode_process[MENGINE_MAX_PATH] = {L'\0'};
        Helper::utf8ToUnicode( _process, unicode_process, MENGINE_MAX_PATH );

        WChar unicode_command[4096] = {L'\0'};
        unicode_command[0] = ' ';
        Helper::utf8ToUnicode( _command, unicode_command + 1, 4095 );

        if( _wait == true )
        {
            SECURITY_ATTRIBUTES sa;
            sa.nLength = sizeof( sa );
            sa.lpSecurityDescriptor = NULL;
            sa.bInheritHandle = TRUE;

            WChar tempPathBuffer[MENGINE_MAX_PATH] = {L'\0'};
            ::GetTempPath( MENGINE_MAX_PATH, tempPathBuffer );

            WChar tempFileNameBuffer[MENGINE_MAX_PATH] = {L'\0'};
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

            STARTUPINFO startupInfo = {0};
            startupInfo.cb = sizeof( STARTUPINFO );
            startupInfo.dwFlags = STARTF_USESTDHANDLES;
            startupInfo.hStdOutput = hWriteTempFile;
            startupInfo.hStdError = hWriteTempFile;
            startupInfo.hStdInput = NULL;

            PROCESS_INFORMATION processInfo = {0};
            if( ::CreateProcess( unicode_process, unicode_command
                , NULL
                , NULL
                , _wait == true ? TRUE : FALSE
                , NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL
                , NULL
                , &startupInfo
                , &processInfo ) == FALSE )
            {
                DWORD error;
                Char str_error[1024] = {'\0'};
                this->getLastErrorMessage( &error, str_error, 1024 );

                LOGGER_ERROR( "CreateProcess '%s' return error: %s [%lu]"
                    , _process
                    , str_error
                    , error
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

            LOGGER_MESSAGE( "result [%lu]"
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

                Char tempFileBuffer[4096] = {'\0'};

                DWORD dwBytesRead;
                DWORD nNumberOfBytesToRead = MENGINE_MIN( tempFileSize, 4096 );
                BOOL successful = ::ReadFile( hReadTempFile, tempFileBuffer, nNumberOfBytesToRead, &dwBytesRead, NULL );

                if( successful == TRUE )
                {
                    LOGGER_VERBOSE_LEVEL( LM_ERROR, LCOLOR_RED, nullptr, 0 )("%s"
                        , tempFileBuffer
                        );
                }
                else
                {
                    LOGGER_VERBOSE_LEVEL( LM_ERROR, LCOLOR_RED, nullptr, 0 )("invalid read file '%ls'"
                        , tempFileNameBuffer
                        );
                }

                ::CloseHandle( hReadTempFile );
            }

            if( _exitCode != nullptr )
            {
                *_exitCode = (uint32_t)exitCode;
            }
        }
        else
        {
            STARTUPINFO startupInfo = {0};
            startupInfo.cb = sizeof( STARTUPINFO );
            PROCESS_INFORMATION processInfo = {0};
            if( ::CreateProcess( unicode_process, unicode_command
                , NULL
                , NULL
                , FALSE
                , NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL
                , NULL
                , &startupInfo
                , &processInfo ) == FALSE )
            {
                DWORD error;
                Char str_error[1024] = {'\0'};
                this->getLastErrorMessage( &error, str_error, 1024 );

                LOGGER_ERROR( "CreateProcess '%s' return error: %s [%lu]"
                    , _process
                    , str_error
                    , error
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::abort()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_ABORT );

        ::abort();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::debugBreak()
    {
        if( ::IsDebuggerPresent() == TRUE )
        {
            ::DebugBreak();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    DynamicLibraryInterfacePtr Win32Platform::loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentPtr & _doc )
    {
        LOGGER_INFO( "load dynamic library '%s'"
            , _dynamicLibraryName
        );

        Win32DynamicLibraryPtr dynamicLibrary = m_factoryDynamicLibraries
            ->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( dynamicLibrary );

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
    bool Win32Platform::getDesktopResolution( Resolution * const _resolution ) const
    {
        int32_t cxscreen = ::GetSystemMetrics( SM_CXSCREEN );
        int32_t cyscreen = ::GetSystemMetrics( SM_CYSCREEN );

        _resolution->setWidth( cxscreen );
        _resolution->setHeight( cyscreen );

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
    size_t Win32Platform::getCurrentPath( Char * const _currentPath ) const
    {
        WChar currentPath[MENGINE_MAX_PATH] = {L'\0'};
        DWORD len = ::GetCurrentDirectory( MENGINE_MAX_PATH, currentPath );

        if( len == 0 )
        {
            _currentPath[0] = '\0';

            return 0;
        }

        Helper::pathCorrectBackslashW( currentPath );

        currentPath[len] = MENGINE_PATH_WDELIM;
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
    size_t Win32Platform::getUserPath( Char * const _userPath ) const
    {
        bool developmentMode = HAS_OPTION( "dev" );
        bool roamingMode = HAS_OPTION( "roaming" );
        bool noroamingMode = HAS_OPTION( "noroaming" );

        if( (developmentMode == true && roamingMode == false) || noroamingMode == true )
        {
            WChar currentPath[MENGINE_MAX_PATH] = {L'\0'};
            DWORD len = ::GetCurrentDirectory( MENGINE_MAX_PATH, currentPath );

            if( len == 0 )
            {
                LOGGER_ERROR( "failed to get current directory"
                );

                return 0;
            }

            ::PathRemoveBackslash( currentPath );

            Helper::pathCorrectBackslashW( currentPath );

            MENGINE_WCSCAT( currentPath, L"/" );
            MENGINE_WCSCAT( currentPath, MENGINE_DEVELOPMENT_USER_FOLDER_NAME );
            MENGINE_WCSCAT( currentPath, L"/" );

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
            Char str_hr[4096] = {'\0'};
            this->getErrorMessage( hr, str_hr, 4096 );

            LOGGER_ERROR( "SHGetSpecialFolderLocation invalid error: %s [%ld] "
                , str_hr
                , hr
            );

            return 0;
        }

        WChar currentPath[MENGINE_MAX_PATH] = {L'\0'};
        BOOL result = ::SHGetPathFromIDListW( itemIDList, currentPath );

        if( result == FALSE )
        {
            LOGGER_ERROR( "SHGetPathFromIDListW invalid"
            );

            return 0;
        }

        ::CoTaskMemFree( itemIDList );

        const Char * companyName = CONFIG_VALUE( "Project", "Company", "NONAME" );

        WChar companyNameW[MENGINE_APPLICATION_COMPANY_MAXNAME] = {L'\0'};
        if( Helper::utf8ToUnicode( companyName, companyNameW, MENGINE_APPLICATION_COMPANY_MAXNAME ) == false )
        {
            return 0;
        }

        WChar roamingPath[MENGINE_MAX_PATH] = {L'\0'};
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

        MENGINE_WCSCAT( roamingPath, L"/" );

        size_t currentPathLen;
        if( Helper::unicodeToUtf8( roamingPath, _userPath, MENGINE_MAX_PATH, &currentPathLen ) == false )
        {
            return 0;
        }

        return currentPathLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32Platform::getUserName( Char * const _userName ) const
    {
        WChar unicode_userName[UNLEN + 1] = {0};
        DWORD unicode_userNameLen = UNLEN + 1;
        if( ::GetUserName( unicode_userName, &unicode_userNameLen ) == FALSE )
        {
            return 0;
        }

        size_t userNameLen;
        if( Helper::unicodeToUtf8Size( unicode_userName, unicode_userNameLen, _userName, MENGINE_PLATFORM_USER_MAXNAME + 1, &userNameLen ) == false )
        {
            return 0;
        }

        return userNameLen;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::minimizeWindow()
    {
        ::ShowWindow( m_hWnd, SW_MINIMIZE );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::setCursorPosition( const mt::vec2f & _pos )
    {
        POINT cPos = {(int32_t)_pos.x, (int32_t)_pos.y};
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
        if( this->calcCursorPosition_( &point ) == true )
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
    //////////////////////////////////////////////////////////////////////////
    UnknownPointer Win32Platform::getPlatformExtention()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Win32Platform::addWin32ProcessHandler( const LambdaWin32ProcessHandler & _lambda )
    {
        uint32_t id = GENERATE_UNIQUE_IDENTITY();

        Win32ProcessDesc desc;
        desc.id = id;
        desc.lambda = _lambda;

        m_win32ProcessHandlers.emplace_back( desc );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::removeWin32ProcessHandler( uint32_t _id )
    {
        VectorWin32ProcessHandler::iterator it_found = std::find_if( m_win32ProcessHandlers.begin(), m_win32ProcessHandlers.end(), [_id]( const Win32ProcessDesc & _desc )
        {
            return _desc.id == _id;
        } );

        MENGINE_ASSERTION_FATAL( it_found != m_win32ProcessHandlers.end() );

        m_win32ProcessHandlers.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    time_t Win32Platform::getFileUnixTime( const FILETIME * filetime ) const
    {
        uint32_t a2 = filetime->dwHighDateTime;
        uint32_t a1 = ((uint32_t)filetime->dwLowDateTime) >> 16;
        uint32_t a0 = ((uint32_t)filetime->dwLowDateTime) & 0xffff;

        uint32_t carry;

        if( a0 >= 32768 )
        {
            a0 -= 32768, carry = 0;
        }
        else
        {
            a0 += (1 << 16) - 32768, carry = 1;
        }

        if( a1 >= 54590 + carry )
        {
            a1 -= 54590 + carry, carry = 0;
        }
        else
        {
            a1 += (1 << 16) - 54590 - carry, carry = 1;
        }

        a2 -= 27111902 + carry;

        int32_t negative = (a2 >= ((uint32_t)1) << 31);

        if( negative != 0 )
        {
            a0 = 0xffff - a0;
            a1 = 0xffff - a1;
            a2 = ~a2;
        }

        a1 += (a2 % 10000) << 16;
        a2 /= 10000;
        a0 += (a1 % 10000) << 16;
        a1 /= 10000;
        a0 /= 10000;

        a1 += (a2 % 1000) << 16;
        a2 /= 1000;
        a0 += (a1 % 1000) << 16;
        a1 /= 1000;
        a0 /= 1000;

        if( negative != 0 )
        {
            a0 = 0xffff - a0;
            a1 = 0xffff - a1;
            a2 = ~a2;
        }

        time_t time = ((((time_t)a2) << 16) << 16) + ((time_t)a1 << 16) + a0;

        return time;
    }
}
