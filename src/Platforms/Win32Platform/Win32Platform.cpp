#include "Win32Platform.h"

#include "Interface/OptionsServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/TimeSystemInterface.h"
#include "Interface/EnumeratorServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/ProfilerSystemInterface.h"
#include "Interface/PluginInterface.h"
#include "Interface/ServiceInterface.h"

#include "Win32CPUInfo.h"
#include "Win32DynamicLibrary.h"
#include "Win32DateTimeProvider.h"

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
#include "Kernel/BuildMode.h"
#include "Kernel/StringArguments.h"
#include "Kernel/Win32Helper.h"
#include "Kernel/ProfilerHelper.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Stringstream.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"
#include "Config/StdIntTypes.h"
#include "Config/Utf8.h"
#include "Config/Algorithm.h"

#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(push, 0) 
#endif

#include "Environment/Windows/DbgHelp.h"

#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(pop) 
#endif

#include <clocale>
#include <ctime>
#include <functional>
#include <cerrno>

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_SETLOCALE
#define MENGINE_SETLOCALE "C"
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_DEVELOPMENT_USER_FOLDER_NAME
#define MENGINE_DEVELOPMENT_USER_FOLDER_NAME L"User"
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_WINDOW_CLASSNAME
#define MENGINE_WINDOW_CLASSNAME "MengineWindow"
#endif
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT( Win32FileGroup );
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
        , m_hIcon( NULL )
        , m_close( false )
        , m_pauseUpdatingTime( -1.f )
        , m_prevTime( 0 )
        , m_cursorInArea( false )
        , m_cursorMode( false )
        , m_cursor( nullptr )
        , m_lastMouse( false )
        , m_lastMouseX( 0 )
        , m_lastMouseY( 0 )
        , m_touchpad( false )
        , m_desktop( false )
        , m_fullscreen( false )
        , m_sessionLock( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32Platform::~Win32Platform()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::_initializeService()
    {
        ::setlocale( LC_ALL, MENGINE_SETLOCALE );

        HMODULE hm_ntdll = ::LoadLibrary( L"ntdll.dll" );

        if( hm_ntdll != NULL )
        {
            LONG( WINAPI * RtlGetVersion )(LPOSVERSIONINFOEXW);
            *(FARPROC *)&RtlGetVersion = GetProcAddress( hm_ntdll, "RtlGetVersion" );

            if( RtlGetVersion != NULL )
            {
                OSVERSIONINFOEXW osInfo;
                osInfo.dwOSVersionInfoSize = sizeof( osInfo );

                RtlGetVersion( &osInfo );

                LOGGER_MESSAGE_RELEASE( "Windows version: %lu.%lu (build %lu)"
                    , osInfo.dwMajorVersion
                    , osInfo.dwMinorVersion
                    , osInfo.dwBuildNumber
                );

                LOGGER_MESSAGE_RELEASE( "Windows platform: %lu"
                    , osInfo.dwPlatformId
                );

                LOGGER_MESSAGE_RELEASE( "Windows service pack: %lu.%lu"
                    , (DWORD)osInfo.wServicePackMajor
                    , (DWORD)osInfo.wServicePackMinor
                );
            }

            ::FreeLibrary( hm_ntdll );
        }

        m_hInstance = ::GetModuleHandle( NULL );

        if( ::QueryPerformanceFrequency( &m_performanceFrequency ) == TRUE )
        {
            m_performanceSupport = true;
        }

        m_cursors[STRINGIZE_STRING_LOCAL( "IDC_ARROW" )] = {::LoadCursor( NULL, IDC_ARROW ), true};
        m_cursors[STRINGIZE_STRING_LOCAL( "IDC_UPARROW" )] = {::LoadCursor( NULL, IDC_UPARROW ), true};
        m_cursors[STRINGIZE_STRING_LOCAL( "IDC_HAND" )] = {::LoadCursor( NULL, IDC_HAND ), true};
        m_cursors[STRINGIZE_STRING_LOCAL( "IDC_HELP" )] = {::LoadCursor( NULL, IDC_HELP ), true};

        m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );

#ifdef MENGINE_PLATFORM_WINDOWS32
        m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN32" ) );
#endif

#ifdef MENGINE_PLATFORM_WINDOWS64
        m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN64" ) );
#endif

        m_touchpad = false;
        m_desktop = true;

        if( HAS_OPTION( "win32" ) == true )
        {
            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN32" ) );

            m_touchpad = false;
            m_desktop = true;
        }
        else if( HAS_OPTION( "win64" ) == true )
        {
            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN64" ) );

            m_touchpad = false;
            m_desktop = true;
        }
        else if( HAS_OPTION( "mac" ) == true )
        {
            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "MAC" ) );

            m_touchpad = false;
            m_desktop = true;
        }
        else if( HAS_OPTION( "ios" ) == true )
        {
            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "IOS" ) );

#ifdef MENGINE_PLATFORM_WINDOWS32
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN32" ) );
#endif

#ifdef MENGINE_PLATFORM_WINDOWS64
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN64" ) );
#endif

            m_touchpad = true;
            m_desktop = false;
        }
        else if( HAS_OPTION( "android" ) == true )
        {
            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "ANDROID" ) );

            m_touchpad = true;
            m_desktop = false;
        }
        else if( HAS_OPTION( "wp" ) == true )
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
            Helper::toupper( option_platform, uppercase_option_platform, 255 );

            m_platformTags.clear();
            m_platformTags.addTag( Helper::stringizeString( option_platform ) );
        }

        if( HAS_OPTION( "touchpad" ) == true )
        {
            m_touchpad = true;
        }

        if( HAS_OPTION( "desktop" ) == true )
        {
            m_desktop = true;
        }

        m_factoryDynamicLibraries = Helper::makeFactoryPool<Win32DynamicLibrary, 8>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryDateTimeProviders = Helper::makeFactoryPool<Win32DateTimeProvider, 8>( MENGINE_DOCUMENT_FACTORABLE );

        const Char * windowClassName = CONFIG_VALUE( "Window", "ClassName", MENGINE_WINDOW_CLASSNAME );

        Helper::utf8ToUnicode( windowClassName, m_windowClassName, MENGINE_MAX_PATH );

        SERVICE_WAIT( FileServiceInterface, [this]()
        {
            if( this->initializeFileService_() == false )
            {
                return false;
            }

            return true;
        } );

        SERVICE_LEAVE( FileServiceInterface, [this]()
        {
            this->finalizeFileService_();
        } );

#if MENGINE_WINDOWS_VERSION >= _WIN32_WINNT_VISTA
        if( HAS_OPTION( "workdir" ) == true )
        {
            Char currentPath[MENGINE_MAX_PATH] = {'\0'};
            if( this->getCurrentPath( currentPath ) == 0 )
            {
                LOGGER_ERROR( "invalid get current path for dll directory" );

                return false;
            }

            WChar currentPathW[MENGINE_MAX_PATH];
            if( Helper::utf8ToUnicode( currentPath, currentPathW, MENGINE_MAX_PATH, nullptr ) == false )
            {
                return false;
            }

            if( ::SetDllDirectoryW( currentPathW ) == FALSE )
            {
                LOGGER_ERROR( "SetDllDirectoryA invalid %s"
                    , Helper::Win32GetLastErrorMessage()
                );

                return false;
            }
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::_finalizeService()
    {
        m_active = false;

        m_platformTags.clear();

        ::SetCursor( NULL );

        for( auto && [name, desc] : m_cursors )
        {
            if( desc.shared == true )
            {
                continue;
            }

            ::DestroyCursor( desc.cursor );
        }

        m_cursors.clear();

#ifdef MENGINE_DEBUG
        for( const Win32ProcessDesc & desc : m_win32ProcessHandlers )
        {
            LOGGER_ERROR( "forgot remove win32 process handler (doc: %s)"
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#endif

        m_win32ProcessHandlers.clear();

#ifdef MENGINE_DEBUG
        for( const TimerDesc & desc : m_timers )
        {
            if( desc.id == INVALID_UNIQUE_ID )
            {
                continue;
            }

            LOGGER_ERROR( "forgot remove win32 timer (doc: %s)"
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#endif

        m_timers.clear();

        if( m_hIcon != NULL )
        {
            ::DestroyIcon( m_hIcon );
            m_hIcon = NULL;
        }

        if( m_hWnd != NULL )
        {
            ::CloseWindow( m_hWnd );
            ::DestroyWindow( m_hWnd );

            m_hWnd = NULL;

            this->updateWndMessage_();
        }

        if( m_hInstance != NULL )
        {
            if( ::UnregisterClass( m_windowClassName, m_hInstance ) == FALSE )
            {
                LOGGER_ERROR( "invalid UnregisterClass '%ls'"
                    , m_windowClassName
                );
            }

            m_hInstance = NULL;
        }

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDynamicLibraries );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDateTimeProviders );

        m_factoryDynamicLibraries = nullptr;
        m_factoryDateTimeProviders = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::_runService()
    {
        DateTimeProviderInterfacePtr dateTimeProvider =
            this->createDateTimeProvider( MENGINE_DOCUMENT_FACTORABLE );

        PlatformDateTime dateTime;
        dateTimeProvider->getLocalDateTime( &dateTime );

        LOGGER_MESSAGE_RELEASE( "Start Date: %02u.%02u.%u, %02u:%02u:%02u"
            , dateTime.day
            , dateTime.month
            , dateTime.year
            , dateTime.hour
            , dateTime.minute
            , dateTime.second
        );

        SYSTEM_INFO sysInfo;
        ::GetSystemInfo( &sysInfo );

        LOGGER_MESSAGE_RELEASE( "Hardware information:" );
        LOGGER_MESSAGE_RELEASE( "  OEM ID: %lu", sysInfo.dwOemId );
        LOGGER_MESSAGE_RELEASE( "  Number of processors: %lu", sysInfo.dwNumberOfProcessors );
        LOGGER_MESSAGE_RELEASE( "  Page size: %lu", sysInfo.dwPageSize );
        LOGGER_MESSAGE_RELEASE( "  Processor type: %lu", sysInfo.dwProcessorType );
        LOGGER_MESSAGE_RELEASE( "  Minimum application address: %p", sysInfo.lpMinimumApplicationAddress );
        LOGGER_MESSAGE_RELEASE( "  Maximum application address: %p", sysInfo.lpMaximumApplicationAddress );
        LOGGER_MESSAGE_RELEASE( "  Active processor mask: %" MENGINE_PRDWORD_PTR, sysInfo.dwActiveProcessorMask );

        LOGGER_MESSAGE_RELEASE( "CPU information:" );

        Win32CPUInfo cpuinfo;

        LOGGER_MESSAGE_RELEASE( "  Vendor: %s", cpuinfo.Vendor().c_str() );
        LOGGER_MESSAGE_RELEASE( "  Brand: %s", cpuinfo.Brand().c_str() );

        auto support_message = []( const Char * isa_feature, bool is_supported )
        {
            uint32_t color = (is_supported == true ? Mengine::LCOLOR_GREEN | Mengine::LCOLOR_BLUE : Mengine::LCOLOR_RED);

            LOGGER_VERBOSE_LEVEL( Mengine::ConstString::none(), Mengine::LM_MESSAGE_RELEASE, color, nullptr, 0 )("  %s: %s"
                , isa_feature
                , is_supported == true ? " supported" : " not supported"
                );
        };

        LOGGER_MESSAGE_RELEASE( "CPU instruction:" );
        support_message( "  3DNOW", cpuinfo._3DNOW() );
        support_message( "  3DNOWEXT", cpuinfo._3DNOWEXT() );
        support_message( "  ABM", cpuinfo.ABM() );
        support_message( "  ADX", cpuinfo.ADX() );
        support_message( "  AES", cpuinfo.AES() );
        support_message( "  AVX", cpuinfo.AVX() );
        support_message( "  AVX2", cpuinfo.AVX2() );
        support_message( "  AVX512CD", cpuinfo.AVX512CD() );
        support_message( "  AVX512ER", cpuinfo.AVX512ER() );
        support_message( "  AVX512F", cpuinfo.AVX512F() );
        support_message( "  AVX512PF", cpuinfo.AVX512PF() );
        support_message( "  BMI1", cpuinfo.BMI1() );
        support_message( "  BMI2", cpuinfo.BMI2() );
        support_message( "  CLFSH", cpuinfo.CLFSH() );
        support_message( "  CMPXCHG16B", cpuinfo.CMPXCHG16B() );
        support_message( "  CX8", cpuinfo.CX8() );
        support_message( "  ERMS", cpuinfo.ERMS() );
        support_message( "  F16C", cpuinfo.F16C() );
        support_message( "  FMA", cpuinfo.FMA() );
        support_message( "  FSGSBASE", cpuinfo.FSGSBASE() );
        support_message( "  FXSR", cpuinfo.FXSR() );
        support_message( "  HLE", cpuinfo.HLE() );
        support_message( "  INVPCID", cpuinfo.INVPCID() );
        support_message( "  LAHF", cpuinfo.LAHF() );
        support_message( "  LZCNT", cpuinfo.LZCNT() );
        support_message( "  MMX", cpuinfo.MMX() );
        support_message( "  MMXEXT", cpuinfo.MMXEXT() );
        support_message( "  MONITOR", cpuinfo.MONITOR() );
        support_message( "  MOVBE", cpuinfo.MOVBE() );
        support_message( "  MSR", cpuinfo.MSR() );
        support_message( "  OSXSAVE", cpuinfo.OSXSAVE() );
        support_message( "  PCLMULQDQ", cpuinfo.PCLMULQDQ() );
        support_message( "  POPCNT", cpuinfo.POPCNT() );
        support_message( "  PREFETCHWT1", cpuinfo.PREFETCHWT1() );
        support_message( "  RDRAND", cpuinfo.RDRAND() );
        support_message( "  RDSEED", cpuinfo.RDSEED() );
        support_message( "  RDTSCP", cpuinfo.RDTSCP() );
        support_message( "  RTM", cpuinfo.RTM() );
        support_message( "  SEP", cpuinfo.SEP() );
        support_message( "  SHA", cpuinfo.SHA() );
        support_message( "  SSE", cpuinfo.SSE() );
        support_message( "  SSE2", cpuinfo.SSE2() );
        support_message( "  SSE3", cpuinfo.SSE3() );
        support_message( "  SSE4.1", cpuinfo.SSE41() );
        support_message( "  SSE4.2", cpuinfo.SSE42() );
        support_message( "  SSE4a", cpuinfo.SSE4a() );
        support_message( "  SSSE3", cpuinfo.SSSE3() );
        support_message( "  SYSCALL", cpuinfo.SYSCALL() );
        support_message( "  TBM", cpuinfo.TBM() );
        support_message( "  XOP", cpuinfo.XOP() );
        support_message( "  XSAVE", cpuinfo.XSAVE() );

        MEMORYSTATUSEX mem_st;
        mem_st.dwLength = sizeof( mem_st );

        if( ::GlobalMemoryStatusEx( &mem_st ) == TRUE )
        {
            LOGGER_MESSAGE_RELEASE( "Start Memory: %u.%uMb total, %u.%uMb free, %u.%uMb Page file total, %u.%uMb Page file free"
                , (uint32_t)(mem_st.ullTotalPhys / (1024UL * 1024UL) / 1024UL), (uint32_t)(mem_st.ullTotalPhys / (1024UL * 1024UL) % 1024UL)
                , (uint32_t)(mem_st.ullAvailPhys / (1024UL * 1024UL) / 1024UL), (uint32_t)(mem_st.ullAvailPhys / (1024UL * 1024UL) % 1024UL)
                , (uint32_t)(mem_st.ullTotalPageFile / (1024UL * 1024UL) / 1024UL), (uint32_t)(mem_st.ullTotalPageFile / (1024UL * 1024UL) % 1024UL)
                , (uint32_t)(mem_st.ullAvailPageFile / (1024UL * 1024UL) / 1024UL), (uint32_t)(mem_st.ullAvailPageFile / (1024UL * 1024UL) % 1024UL)
            );
        }

        if( this->setProcessDPIAware() == false )
        {
            LOGGER_MESSAGE_RELEASE( "Application not setup Process DPI Aware" );
        }

        return true;
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
        FARPROC pSetProcessDPIAware = ::GetProcAddress( hUser32, "SetProcessDPIAware" );

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

        if( ::IsDebuggerPresent() == TRUE )
        {
            return false;
        }

        WString unicode_processDumpPath;
        Helper::utf8ToUnicode( _dumpPath, &unicode_processDumpPath );

        HANDLE hFile = ::CreateFile( unicode_processDumpPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0 );

        if( hFile == INVALID_HANDLE_VALUE )
        {
            LOGGER_ERROR( "invalid create file for '%ls' %s"
                , unicode_processDumpPath.c_str()
                , Helper::Win32GetLastErrorMessage()
            );

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

        BOOL successful = (*MiniDumpWriteDump)(hProcess, dwProcessId, hFile, dumptype, (_pExceptionPointers == nullptr ? nullptr : &exinfo), NULL, NULL);

        ::FreeLibrary( dbghelp_dll );
        ::CloseHandle( hFile );

        if( successful == FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId Win32Platform::addTimer( float _milliseconds, const LambdaTimer & _lambda, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        UniqueId new_id = ENUMERATOR_SERVICE()
            ->generateUniqueIdentity();

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
    void Win32Platform::removeTimer( UniqueId _id )
    {
        VectorTimers::iterator it_found = std::find_if( m_timers.begin(), m_timers.end(), [_id]( const TimerDesc & _desc )
        {
            return _desc.id == _id;
        } );

        MENGINE_ASSERTION_FATAL( it_found != m_timers.end(), "not found timer '%u'"
            , _id
        );

        TimerDesc & desc = *it_found;

        desc.id = INVALID_UNIQUE_ID;
        desc.lambda = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t Win32Platform::getTicks() const
    {
        if( m_performanceSupport == false )
        {
            return 0ULL;
        }

        LARGE_INTEGER performanceCount;
        if( ::QueryPerformanceCounter( &performanceCount ) == FALSE )
        {
            LOGGER_ERROR( "invalid query performance counter %s"
                , Helper::Win32GetLastErrorMessage()
            );

            return 0ULL;
        }

        LONGLONG ticks = performanceCount.QuadPart / m_performanceFrequency.QuadPart;

        return (uint64_t)ticks;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::runPlatform()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_RUN );

        LOGGER_MESSAGE_RELEASE( "run platform" );

        MENGINE_PROFILER_BEGIN_APPLICATION();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::updateWndMessage_()
    {
        MENGINE_PROFILER_CATEGORY();

        MSG msg;
        while( ::PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != FALSE )
        {
            ::TranslateMessage( &msg );
            ::DispatchMessage( &msg );
        }
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
                MENGINE_PROFILER_FRAME( "main" );

                uint64_t currentTime = TIME_SYSTEM()
                    ->getTimeMilliseconds();

                float frameTime = (float)(currentTime - m_prevTime);

                m_prevTime = currentTime;

                this->updateWndMessage_();

                if( m_close == true )
                {
                    break;
                }

                NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_UPDATE );

                if( m_sessionLock == true )
                {
                    ::Sleep( 100 );

                    continue;
                }

                for( TimerDesc & desc : m_timers )
                {
                    if( desc.id == INVALID_UNIQUE_ID )
                    {
                        continue;
                    }

                    desc.time -= frameTime;

                    if( desc.time > 0.f )
                    {
                        continue;
                    }

                    desc.time += desc.milliseconds;

                    desc.lambda( desc.id );
                }

                m_timers.erase( std::remove_if( m_timers.begin(), m_timers.end(), []( const TimerDesc & _desc )
                {
                    return _desc.id == INVALID_UNIQUE_ID;
                } ), m_timers.end() );

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
                }

                if( this->isNeedWindowRender() == true )
                {
                    bool sucessful = APPLICATION_SERVICE()
                        ->render();

                    if( sucessful == true )
                    {
                        APPLICATION_SERVICE()
                            ->flush();
                    }
                }

                APPLICATION_SERVICE()
                    ->endUpdate();

                m_update = false;

                if( updating == false )
                {
                    if( m_pauseUpdatingTime < 0.f )
                    {
                        m_pauseUpdatingTime = frameTime;
                    }

                    ::Sleep( 100 );
                }
                else
                {
                    bool OPTION_maxfps = HAS_OPTION( "maxfps" );

                    if( APPLICATION_SERVICE()
                        ->getVSync() == false && OPTION_maxfps == false )
                    {
                        ::Sleep( 1 );
                    }
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::stopPlatform()
    {
        LOGGER_MESSAGE( "stop platform" );

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_STOP );

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

            this->updateWndMessage_();
        }

        MENGINE_PROFILER_END_APPLICATION();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::setHWNDIcon( const WChar * _iconResource )
    {
        HICON hIcon = ::LoadIcon( m_hInstance, _iconResource );

        if( hIcon == NULL )
        {
            LOGGER_ERROR( "invalid load HWND icon '%ls' %s"
                , _iconResource
                , Helper::Win32GetLastErrorMessage()
            );

            return false;
        }

        m_hIcon = hIcon;

        return true;
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
    size_t Win32Platform::getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const
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
            WChar valueName[MENGINE_MAX_PATH] = {L'\0'};
            BYTE valueData[MENGINE_MAX_PATH * 2] = {0};

            result = ::RegEnumValue( hKey, valueIndex, valueName, &valueNameSize, 0, &valueType, valueData, &valueDataSize );

            valueIndex++;

            if( result != ERROR_SUCCESS || valueType != REG_SZ )
            {
                continue;
            }

            // Found a match
            if( MENGINE_WCSICMP( unicode_fontName, valueName ) == 0 )
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

        WChar fullDir[MENGINE_MAX_PATH] = {L'\0'};
        MENGINE_WSPRINTF( fullDir, L"Fonts\\%s"
            , unicode_fontPath
        );

        size_t utf8_size;
        if( Helper::unicodeToUtf8( fullDir, _fontPath, MENGINE_MAX_PATH, &utf8_size ) == false )
        {
            _fontPath[0] = '\0';

            return MENGINE_PATH_INVALID_LENGTH;
        }

        *_groupName = STRINGIZE_STRING_LOCAL( "windows" );

        return utf8_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::getMaxClientResolution( Resolution * const _resolution ) const
    {
        RECT workArea;
        if( ::SystemParametersInfo( SPI_GETWORKAREA, 0, &workArea, 0 ) == FALSE )
        {
            LOGGER_ERROR( "invalid get system parameters info %s"
                , Helper::Win32GetLastErrorMessage()
            );

            return false;
        }

        DWORD dwStyle = this->getWindowStyle_( false );
        DWORD dwExStyle = this->getWindowExStyle_( false );

        RECT clientArea = workArea;

        if( ::AdjustWindowRectEx( &clientArea, dwStyle, FALSE, dwExStyle ) == FALSE )
        {
            LOGGER_ERROR( "invalid adjust window rect %s"
                , Helper::Win32GetLastErrorMessage()
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
    namespace Detail
    {
        static LRESULT CALLBACK wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
        {
            switch( uMsg )
            {
                //case WM_NCCREATE:
            case WM_CREATE:
                {
                    LPCREATESTRUCTW createStruct = (LPCREATESTRUCTW)lParam;

                    Win32Platform * app = (Win32Platform *)createStruct->lpCreateParams;

#ifdef MENGINE_PLATFORM_WINDOWS64
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
            BOOL handled = FALSE;
            LRESULT lResult = desc.lambda( hWnd, uMsg, wParam, lParam, &handled );

            if( handled == TRUE )
            {
                LOGGER_INFO( "platform", "HWND [%p] handled proccess wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u] (doc: %s)"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                    , MENGINE_DOCUMENT_STR( desc.doc )
                );

                return lResult;
            }
        }

        switch( uMsg )
        {
        case WM_ACTIVATE:
            {
                DWORD flagActive = LOWORD( wParam );
                BOOL minimized = (BOOL)HIWORD( wParam );

                LOGGER_INFO( "platform", "HWND [%p] WM_ACTIVATE active [%" MENGINE_PRDWORD "] minimized [%u] visible [%u]"
                    , hWnd
                    , flagActive
                    , minimized
                    , ::IsWindowVisible( hWnd )
                );

                if( flagActive != WA_INACTIVE && minimized == FALSE )
                {
                    if( ::IsWindowVisible( hWnd ) == FALSE )
                    {
                        break;
                    }

                    this->setActive_( true );
                }
                else
                {
                    this->setActive_( false );
                }

                return 0;
            }break;
        case WM_ACTIVATEAPP:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_ACTIVATEAPP wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

            }break;
        case WM_SETFOCUS:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_SETFOCUS wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

            }break;
        case WM_KILLFOCUS:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_KILLFOCUS wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

            }break;
        case WM_PAINT:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_PAINT wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

            }break;
#if MENGINE_WINDOWS_VERSION >= _WIN32_WINNT_VISTA
        case WM_WTSSESSION_CHANGE:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_WTSSESSION_CHANGE wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                switch( wParam )
                {
                case WTS_SESSION_LOCK:
                    {
                        LOGGER_INFO( "platform", "HWND [%p] WTS_SESSION_LOCK"
                            , hWnd
                        );

                        m_sessionLock = true;
                    }break;
                case WTS_SESSION_UNLOCK:
                    {
                        LOGGER_INFO( "platform", "HWND [%p] WTS_SESSION_UNLOCK"
                            , hWnd
                        );

                        m_sessionLock = false;
                    }break;
                }
            }break;
#endif
        case WM_DISPLAYCHANGE:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_DISPLAYCHANGE wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

            }break;
        case WM_SIZE:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_SIZE wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                if( wParam == SIZE_MAXIMIZED )
                {
                    LOGGER_INFO( "platform", "HWND [%p] SIZE_MAXIMIZED"
                        , hWnd
                    );

                    this->setActive_( true );
                }
                else if( wParam == SIZE_MINIMIZED )
                {
                    LOGGER_INFO( "platform", "HWND [%p] SIZE_MINIMIZED"
                        , hWnd
                    );

                    this->setActive_( false );
                }
                else if( wParam == SIZE_RESTORED /*&& m_application->getFullscreenMode() == true*/ )
                {
                    LOGGER_INFO( "platform", "HWND [%p] SIZE_RESTORED"
                        , hWnd
                    );

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
                LOGGER_INFO( "platform", "HWND [%p] WM_GETMINMAXINFO wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                // Prevent the window from going smaller than some minimu size
                MINMAXINFO * info = (MINMAXINFO *)lParam;

                info->ptMinTrackSize.x = 100;
                info->ptMinTrackSize.y = 100;
            }break;
        case WM_CLOSE:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_CLOSE wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                m_close = true;

                return 0;
            }break;
        case WM_SYSKEYDOWN:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_SYSKEYDOWN wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

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
                LOGGER_INFO( "platform", "HWND [%p] WM_SYSKEYUP wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

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
                LOGGER_INFO( "platform", "HWND [%p] WM_SYSCOMMAND wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

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
                LOGGER_INFO( "platform", "HWND [%p] WM_SETCURSOR wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

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
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_DESTROY wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                LOGGER_MESSAGE_RELEASE( "Quit application" );

                m_close = true;

                ::PostQuitMessage( 0 );

                return 0;
            }break;
        default:
            {
                LOGGER_INFO( "platform", "HWND [%p] message [%u] wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , uMsg
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                LRESULT input_result;
                if( this->wndProcInput( hWnd, uMsg, wParam, lParam, &input_result ) == true )
                {
                    return input_result;
                }
            }break;
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
                LOGGER_INFO( "platform", "HWND [%p] WM_TIMER wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                if( wParam == MENGINE_UTIMER_MOUSE_EVENT )
                {
                    m_mouseEvent.verify();
                }

                handle = true;
                *_result = 0;
            }break;
        case MENGINE_UWM_MOUSE_LEAVE:
            {
                LOGGER_INFO( "platform", "HWND [%p] UWM_MOUSE_LEAVE wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                if( m_cursorInArea == true )
                {
                    m_cursorInArea = false;
                    ::InvalidateRect( hWnd, NULL, FALSE );
                    ::UpdateWindow( hWnd );

                    POINT p;
                    p.x = GET_X_LPARAM( lParam );
                    p.y = GET_Y_LPARAM( lParam );

                    mt::vec2f point;
                    if( this->getCursorPosition_( p, &point ) == false )
                    {
                        return false;
                    }

                    Helper::pushMouseLeaveEvent( TC_TOUCH0, point.x, point.y, 0.f );

                    if( (::GetKeyState( VK_LBUTTON ) & 0x8000) != 0 )
                    {
                        m_clickOutArea[0] = true;
                    }

                    if( (::GetKeyState( VK_RBUTTON ) & 0x8000) != 0 )
                    {
                        m_clickOutArea[1] = true;
                    }

                    if( (::GetKeyState( VK_MBUTTON ) & 0x8000) != 0 )
                    {
                        m_clickOutArea[2] = true;
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
                LOGGER_INFO( "platform", "HWND [%p] WM_MOUSEMOVE wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                POINT p;
                p.x = GET_X_LPARAM( lParam );
                p.y = GET_Y_LPARAM( lParam );

                mt::vec2f point;
                if( this->getCursorPosition_( p, &point ) == false )
                {
                    return false;
                }

                if( m_clickOutArea[0] == true )
                {
                    m_clickOutArea[0] = false;

                    if( (::GetKeyState( VK_LBUTTON ) & 0x8000) == 0 )
                    {
                        Helper::pushMouseButtonEvent( TC_TOUCH0, point.x, point.y, MC_LBUTTON, 0.f, false );
                    }
                }

                if( m_clickOutArea[1] == true )
                {
                    m_clickOutArea[1] = false;

                    if( (::GetKeyState( VK_RBUTTON ) & 0x8000) == 0 )
                    {
                        Helper::pushMouseButtonEvent( TC_TOUCH0, point.x, point.y, MC_RBUTTON, 0.f, false );
                    }
                }

                if( m_clickOutArea[2] == true )
                {
                    m_clickOutArea[2] = false;

                    if( (::GetKeyState( VK_MBUTTON ) & 0x8000) == 0 )
                    {
                        Helper::pushMouseButtonEvent( TC_TOUCH0, point.x, point.y, MC_MBUTTON, 0.f, false );
                    }
                }

                if( m_cursorInArea == false && m_fullscreen == false )
                {
                    m_cursorInArea = true;

                    m_mouseEvent.update();

                    ::InvalidateRect( hWnd, NULL, FALSE );
                    ::UpdateWindow( hWnd );

                    Helper::pushMouseEnterEvent( TC_TOUCH0, point.x, point.y, 0.f );
                }

                if( m_lastMouse == false )
                {
                    m_lastMouseX = p.x;
                    m_lastMouseY = p.y;

                    m_lastMouse = true;
                }

                int32_t dx = p.x - m_lastMouseX;
                int32_t dy = p.y - m_lastMouseY;

                if( dx == 0 && dy == 0 )
                {
                    break;
                }

                m_lastMouseX = p.x;
                m_lastMouseY = p.y;

                float fdx = (float)dx;
                float fdy = (float)dy;

                RECT rect;
                if( ::GetClientRect( m_hWnd, &rect ) == FALSE )
                {
                    LOGGER_ERROR( "invalid hwnd [%p] get client rect %s"
                        , m_hWnd
                        , Helper::Win32GetLastErrorMessage()
                    );

                    return false;
                }

                float width = static_cast<float>(rect.right - rect.left);
                float height = static_cast<float>(rect.bottom - rect.top);

                fdx /= width;
                fdy /= height;

                Helper::pushMouseMoveEvent( TC_TOUCH0, point.x, point.y, fdx, fdy, 0.f );

                handle = true;
                *_result = 0;
            }break;
        case WM_MOUSEWHEEL:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_MOUSEWHEEL wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                int32_t fwKeys = GET_KEYSTATE_WPARAM( wParam );
                MENGINE_UNUSED( fwKeys );

                int32_t zDelta = GET_WHEEL_DELTA_WPARAM( wParam );

                POINT p;
                p.x = GET_X_LPARAM( lParam );
                p.y = GET_Y_LPARAM( lParam );

                mt::vec2f point;
                if( this->adaptCursorPosition_( p, &point ) == false ) //indeed carries screen-based coordinates in lParam
                {
                    return false;
                }

                int32_t scroll = zDelta / WHEEL_DELTA;

                Helper::pushMouseWheelEvent( point.x, point.y, WC_CENTRAL, scroll );

                handle = true;
                *_result = 0;
            }break;
        case WM_LBUTTONDBLCLK:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_LBUTTONDBLCLK wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                m_isDoubleClick[0] = true;

                handle = true;
                *_result = 0;
            }break;
        case WM_RBUTTONDBLCLK:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_RBUTTONDBLCLK wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                m_isDoubleClick[1] = true;

                handle = true;
                *_result = 0;
            }break;
        case WM_MBUTTONDBLCLK:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_MBUTTONDBLCLK wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                m_isDoubleClick[2] = true;

                handle = true;
                *_result = 0;
            }break;
        case WM_LBUTTONDOWN:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_LBUTTONDOWN wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                POINT p;
                p.x = GET_X_LPARAM( lParam );
                p.y = GET_Y_LPARAM( lParam );

                mt::vec2f point;
                if( this->getCursorPosition_( p, &point ) == false )
                {
                    return false;
                }

                Helper::pushMouseButtonEvent( TC_TOUCH0, point.x, point.y, MC_LBUTTON, 0.f, true );

                handle = true;
                *_result = 0;
            }
            break;
        case WM_LBUTTONUP:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_LBUTTONUP wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                if( m_isDoubleClick[0] == false )
                {
                    POINT p;
                    p.x = GET_X_LPARAM( lParam );
                    p.y = GET_Y_LPARAM( lParam );

                    mt::vec2f point;
                    if( this->getCursorPosition_( p, &point ) == false )
                    {
                        return false;
                    }

                    Helper::pushMouseButtonEvent( TC_TOUCH0, point.x, point.y, MC_LBUTTON, 0.f, false );
                }

                m_isDoubleClick[0] = false;

                handle = true;
                *_result = 0;
            }break;
        case WM_RBUTTONDOWN:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_RBUTTONDOWN wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                POINT p;
                p.x = GET_X_LPARAM( lParam );
                p.y = GET_Y_LPARAM( lParam );

                mt::vec2f point;
                if( this->getCursorPosition_( p, &point ) == false )
                {
                    return false;
                }

                Helper::pushMouseButtonEvent( TC_TOUCH0, point.x, point.y, MC_RBUTTON, 0.f, true );

                handle = true;
                *_result = 0;
            }break;
        case WM_RBUTTONUP:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_RBUTTONUP wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                if( m_isDoubleClick[1] == false )
                {
                    POINT p;
                    p.x = GET_X_LPARAM( lParam );
                    p.y = GET_Y_LPARAM( lParam );

                    mt::vec2f point;
                    if( this->getCursorPosition_( p, &point ) == false )
                    {
                        return false;
                    }

                    Helper::pushMouseButtonEvent( TC_TOUCH0, point.x, point.y, MC_RBUTTON, 0.f, false );
                }

                m_isDoubleClick[1] = false;

                handle = true;
                *_result = 0;
            }break;
        case WM_MBUTTONDOWN:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_MBUTTONDOWN wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                POINT p;
                p.x = GET_X_LPARAM( lParam );
                p.y = GET_Y_LPARAM( lParam );

                mt::vec2f point;
                if( this->getCursorPosition_( p, &point ) == false )
                {
                    return false;
                }

                Helper::pushMouseButtonEvent( TC_TOUCH0, point.x, point.y, MC_MBUTTON, 0.f, true );

                handle = true;
                *_result = 0;
            }break;
        case WM_MBUTTONUP:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_MBUTTONUP wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                if( m_isDoubleClick[2] == false )
                {
                    POINT p;
                    p.x = GET_X_LPARAM( lParam );
                    p.y = GET_Y_LPARAM( lParam );

                    mt::vec2f point;
                    if( this->getCursorPosition_( p, &point ) == false )
                    {
                        return false;
                    }

                    Helper::pushMouseButtonEvent( TC_TOUCH0, point.x, point.y, MC_MBUTTON, 0.f, false );
                }

                m_isDoubleClick[2] = false;

                handle = true;
                *_result = 0;
            }break;
        case WM_KEYDOWN:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_KEYDOWN wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                UINT vkc = static_cast<UINT>(wParam);

                mt::vec2f point;
                if( this->calcCursorPosition_( &point ) == false )
                {
                    return false;
                }

                EKeyCode code = (EKeyCode)vkc;

                Helper::pushKeyEvent( point.x, point.y, code, true, false );

                handle = true;
                *_result = 0;
            }break;
        case WM_KEYUP:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_KEYUP wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                UINT vkc = static_cast<UINT>(wParam);

                mt::vec2f point;
                if( this->calcCursorPosition_( &point ) == false )
                {
                    return false;
                }

                EKeyCode code = (EKeyCode)vkc;

                Helper::pushKeyEvent( point.x, point.y, code, false, false );

                handle = true;
                *_result = 0;
            }break;
        case WM_UNICHAR:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_UNICHAR wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                if( wParam == UNICODE_NOCHAR )
                {
                    handle = true;
                    *_result = 1;

                    break;
                }

                if( this->sendChar_( wParam ) == false )
                {
                    return false;
                }

                handle = true;
                *_result = 0;
            }break;
        case WM_CHAR:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_CHAR wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%u]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                if( this->sendChar_( wParam ) == false )
                {
                    return false;
                }

                handle = true;
                *_result = 0;
            }break;
        default:
            {
            }break;
        }

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::sendChar_( WPARAM wParam )
    {
        Char utf8[5] = {'\0'};
        if( s_sonvertUTF32toUTF8( (uint32_t)wParam, utf8 ) == false )
        {
            return false;
        }

        mt::vec2f point;
        if( this->calcCursorPosition_( &point ) == false )
        {
            return false;
        }

        WChar text_code[2] = {L'\0'};
        if( Helper::utf8ToUnicode( utf8, text_code, 2 ) == false )
        {
            return false;
        }

        Helper::pushTextEvent( point.x, point.y, text_code[0] );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::alreadyRunningMonitor()
    {
        bool Platform_AlreadyRunning = CONFIG_VALUE( "Platform", "AlreadyRunning", true );

        if( Platform_AlreadyRunning == true && HAS_OPTION( "noalreadyrunning" ) == false )
        {
            m_alreadyRunningMonitor = Helper::makeFactorableUnique<Win32AlreadyRunningMonitor>( MENGINE_DOCUMENT_FACTORABLE );

            bool stop;
            if( m_alreadyRunningMonitor->initialize( EARP_SETFOCUS, m_windowClassName, m_projectTitle, &stop ) == false )
            {
                LOGGER_ERROR( "invalid initialize already running monitor" );

                return true;
            }

            if( stop == true )
            {
                m_close = true;

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::createWindow( const Resolution & _resolution, bool _fullscreen )
    {
        m_windowResolution = _resolution;
        m_fullscreen = _fullscreen;

        bool Platform_WithoutWindow = CONFIG_VALUE( "Platform", "WithoutWindow", false );

        if( Platform_WithoutWindow == true || HAS_OPTION( "norender" ) == true )
        {
            LOGGER_MESSAGE( "platform without window" );

            return true;
        }

        WNDCLASSEX wc;
        ::ZeroMemory( &wc, sizeof( WNDCLASSEX ) );
        wc.cbSize = sizeof( WNDCLASSEX );
        wc.style = /*CS_DBLCLKS |*/ CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = &Detail::wndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = m_hInstance;

        wc.hIcon = m_hIcon;
        wc.hCursor = ::LoadCursor( NULL, MAKEINTRESOURCEW( 32512 ) );
        wc.hbrBackground = (HBRUSH)::GetStockObject( BLACK_BRUSH );

        wc.lpszMenuName = NULL;
        wc.lpszClassName = m_windowClassName;
        wc.hIconSm = NULL;

        ATOM result = ::RegisterClassEx( &wc );

        if( result == 0 )
        {
            LOGGER_ERROR( "Can't register window class %s"
                , Helper::Win32GetLastErrorMessage()
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

        HWND hWnd = ::CreateWindowEx( dwExStyle, m_windowClassName, m_projectTitle
            , dwStyle
            , rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top
            , NULL, NULL, m_hInstance, (LPVOID)this );

        if( hWnd == NULL )
        {
            LOGGER_ERROR( "can't create window %s"
                , Helper::Win32GetLastErrorMessage()
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

#if MENGINE_WINDOWS_VERSION >= _WIN32_WINNT_VISTA
        if( ::WTSRegisterSessionNotification( m_hWnd, NOTIFY_FOR_ALL_SESSIONS ) == FALSE )
        {
            LOGGER_ERROR( "invalid hwnd [%p] register session notification %s"
                , m_hWnd
                , Helper::Win32GetLastErrorMessage()
            );
        }
#endif

        if( _fullscreen == true )
        {
            Resolution desktopResolution;
            this->getDesktopResolution( &desktopResolution );

            m_windowResolution = desktopResolution;
        }

        ::SetForegroundWindow( m_hWnd );
        ::SetFocus( m_hWnd );
        ::UpdateWindow( m_hWnd );

        m_mouseEvent.setHWND( m_hWnd );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    HWND Win32Platform::getWindowHandle() const
    {
        return m_hWnd;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
        LOGGER_MESSAGE( "resolution %u:%u fullscreen [%u]"
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
            return false;
        }

        LONG dwExStyle = ::GetWindowLong( m_hWnd, GWL_EXSTYLE );

        HWND hWndTop;

        if( m_fullscreen == false )
        {
            LONG dwExStyleNew = dwExStyle & (~dwFullscreenExStyle);

            ::SetWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyleNew );

            hWndTop = HWND_NOTOPMOST;
        }
        else
        {
            LONG dwExStyleNew = dwExStyle | dwFullscreenExStyle;

            ::SetWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyleNew );

            hWndTop = HWND_TOPMOST;
        }

        ::SetWindowLong( m_hWnd, GWL_STYLE, dwStyle );

        ::SetWindowPos(
            m_hWnd
            , hWndTop
            , rc.left
            , rc.top
            , rc.right - rc.left
            , rc.bottom - rc.top
            , SWP_NOCOPYBITS | SWP_NOACTIVATE
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::notifyVsyncChanged( bool _vsync )
    {
        MENGINE_UNUSED( _vsync );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::notifyCursorModeChanged( bool _mode )
    {
        m_cursorMode = _mode;

        if( m_cursor == NULL )
        {
            m_cursor = ::LoadCursor( NULL, IDC_ARROW );
        }

        m_cursor = m_cursorMode == true ? m_cursor : NULL;

        ::SetCursor( m_cursor );
    }
    //////////////////////////////////////////////////////////////////////////
    HCURSOR Win32Platform::loadCursorICO_( const FilePath & _filePath, const MemoryInterfacePtr & _buffer ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _buffer );

        if( _buffer->empty() == true )
        {
            LOGGER_ERROR( "'%s' buffer empty"
                , _filePath.c_str()
            );

            return NULL;
        }

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        PathString icoFile;
        icoFile += ".icon_cache/";
        icoFile += _filePath;
        icoFile += ".ico";

        FilePath c_icoFile = Helper::stringizeFilePath( icoFile );

        if( fileGroup->createDirectory( c_icoFile ) == false )
        {
            LOGGER_ERROR( "invalid create directory '.icon_cache'" );

            return NULL;
        }

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( fileGroup, c_icoFile, true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "path '%s' can't open output stream '%s'"
            , _filePath.c_str()
            , c_icoFile.c_str()
        );

        void * memory = _buffer->getBuffer();
        size_t size = _buffer->getSize();

        if( stream->write( memory, size ) == false )
        {
            LOGGER_ERROR( "path '%s' can't write output stream '%s'"
                , _filePath.c_str()
                , c_icoFile.c_str()
            );

            return NULL;
        }

        stream->flush();
        stream = nullptr;

        Char icoFullFile[MENGINE_MAX_PATH] = {L'\0'};
        fileGroup->getFullPath( c_icoFile, icoFullFile );

        WString unicode_icoFullFile;
        if( Helper::utf8ToUnicode( icoFullFile, &unicode_icoFullFile ) == false )
        {
            LOGGER_ERROR( "path '%s' can't file name '%s' to unicode"
                , _filePath.c_str()
                , c_icoFile.c_str()
            );

            return NULL;
        }

        const WChar * unicode_icoFile_str = unicode_icoFullFile.c_str();

        HCURSOR cursor = ::LoadCursorFromFileW( unicode_icoFile_str );

        if( cursor == NULL )
        {
            LOGGER_ERROR( "icon file '%ls' %s"
                , unicode_icoFullFile.c_str()
                , Helper::Win32GetLastErrorMessage()
            );

            return NULL;
        }

        return cursor;
    }
    //////////////////////////////////////////////////////////////////////////
    HCURSOR Win32Platform::getCursorICO_( const ConstString & _name, const FilePath & _filePath, const MemoryInterfacePtr & _buffer )
    {
        MapCursors::iterator it_found = m_cursors.find( _filePath );

        if( it_found != m_cursors.end() )
        {
            const CursorDesc & desc = it_found->second;

            HCURSOR cursor = desc.cursor;

            return cursor;
        }

        HCURSOR cursor = this->loadCursorICO_( _filePath, _buffer );

        if( cursor == NULL )
        {
            LOGGER_ERROR( "invalid load cursor ICO [%s]"
                , _name.c_str()
            );

            cursor = ::LoadCursor( NULL, IDC_ARROW );
        }

        CursorDesc desc;
        desc.cursor = cursor;
        desc.shared = false;

        m_cursors.emplace( _name, desc );

        return cursor;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::notifyCursorIconSetup( const ConstString & _name, const ContentInterfacePtr & _content, const MemoryInterfacePtr & _buffer )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _content );

        const FilePath & filePath = _content->getFilePath();

        HCURSOR cursor = this->getCursorICO_( _name, filePath, _buffer );

        m_cursor = cursor;

        ::SetCursor( m_cursor );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    DWORD Win32Platform::getWindowStyle_( bool _fullsreen ) const
    {
        MENGINE_UNUSED( _fullsreen );

        DWORD dwStyle = WS_OVERLAPPED;

        dwStyle |= WS_POPUP;
        dwStyle |= WS_VISIBLE;

        if( _fullsreen == false )
        {
            dwStyle |= WS_CAPTION;

            dwStyle |= WS_SYSMENU;
            dwStyle |= WS_MINIMIZEBOX;
        }

        return dwStyle;
    }
    //////////////////////////////////////////////////////////////////////////
    DWORD Win32Platform::getWindowExStyle_( bool _fullsreen ) const
    {
        MENGINE_UNUSED( _fullsreen );

        DWORD dwExStyle = 0;

        //ToDo

        dwExStyle |= WS_EX_APPWINDOW;

        if( _fullsreen == false )
        {
            dwExStyle |= WS_EX_WINDOWEDGE;
        }

        return dwExStyle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::calcWindowsRect_( const Resolution & _resolution, bool _fullsreen, RECT * const _rect ) const
    {
        uint32_t resolutionWidth = _resolution.getWidth();
        uint32_t resolutionHeight = _resolution.getHeight();

        RECT rc;
        if( ::SetRect( &rc, 0, 0, (int32_t)resolutionWidth, (int32_t)resolutionHeight ) == FALSE )
        {
            LOGGER_ERROR( "invalid set rect %s"
                , Helper::Win32GetLastErrorMessage()
            );

            return false;
        }

        if( _fullsreen == false )
        {
            DWORD dwStyle = this->getWindowStyle_( _fullsreen );
            DWORD dwStyleEx = this->getWindowExStyle_( _fullsreen );

            if( ::AdjustWindowRectEx( &rc, dwStyle, FALSE, dwStyleEx ) == FALSE )
            {
                LOGGER_ERROR( "invalid adjust window rect %s"
                    , Helper::Win32GetLastErrorMessage()
                );

                return false;
            }

            RECT workArea;
            if( ::SystemParametersInfo( SPI_GETWORKAREA, 0, &workArea, 0 ) == FALSE )
            {
                LOGGER_ERROR( "invalid system parameters info %s"
                    , Helper::Win32GetLastErrorMessage()
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
    bool Win32Platform::calcCursorPosition_( mt::vec2f * const _position ) const
    {
        POINT point;
        if( ::GetCursorPos( &point ) == FALSE )
        {
            LOGGER_ERROR( "invalid get cursor pos %s"
                , Helper::Win32GetLastErrorMessage()
            );

            return false;
        }

        bool result = this->adaptCursorPosition_( point, _position );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::adaptCursorPosition_( POINT _point, mt::vec2f * const _position ) const
    {
        POINT cPos = _point;
        if( ::ScreenToClient( m_hWnd, &cPos ) == FALSE )
        {
            LOGGER_ERROR( "invalid hwnd [%p] screen [%lu %lu] to client %s"
                , m_hWnd
                , _point.x
                , _point.y
                , Helper::Win32GetLastErrorMessage()
            );

            return false;
        }

        bool result = this->getCursorPosition_( cPos, _position );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::getCursorPosition_( POINT _clientPoint, mt::vec2f * const _position ) const
    {
        float x = static_cast<float>(_clientPoint.x);
        float y = static_cast<float>(_clientPoint.y);

        RECT rect;
        if( ::GetClientRect( m_hWnd, &rect ) == FALSE )
        {
            LOGGER_ERROR( "invalid hwnd [%p] get client rect %s"
                , m_hWnd
                , Helper::Win32GetLastErrorMessage()
            );

            return false;
        }

        float width = static_cast<float>(rect.right - rect.left);
        float height = static_cast<float>(rect.bottom - rect.top);

        _position->x = x / width;
        _position->y = y / height;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::existDirectory( const Char * _path, const Char * _directory ) const
    {
        MENGINE_ASSERTION_FATAL( MENGINE_STRLEN( _path ) == 0 || (MENGINE_STRRCHR( _path, '.' ) > MENGINE_STRRCHR( _path, MENGINE_PATH_DELIM ) || _path[MENGINE_STRLEN( _path ) - 1] == '/') );

        MENGINE_ASSERTION_FATAL( MENGINE_STRLEN( _directory ) == 0 || (MENGINE_STRRCHR( _directory, '.' ) > MENGINE_STRRCHR( _directory, MENGINE_PATH_DELIM ) || _directory[MENGINE_STRLEN( _directory ) - 1] == '/') );

        WChar unicode_path[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar unicode_directory[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _directory, unicode_directory, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar pathDirectory[MENGINE_MAX_PATH] = {L'\0'};
        Helper::pathCorrectBackslashToW( pathDirectory, unicode_directory );

        Helper::pathRemoveFileSpecW( pathDirectory );

        size_t len = MENGINE_WCSLEN( pathDirectory );

        if( len == 0 )
        {
            return true;
        }

        WChar pathFull[MENGINE_MAX_PATH] = {'\0'};
        MENGINE_WNSPRINTF( pathFull, MENGINE_MAX_PATH, L"%ls%ls"
            , _path
            , pathDirectory
        );

        if( pathFull[len - 1] == L':' )
        {
            return true;
        }

        DWORD attributes = ::GetFileAttributes( pathFull );

        if( attributes == INVALID_FILE_ATTRIBUTES )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::createDirectory( const Char * _path, const Char * _directory )
    {
        MENGINE_ASSERTION_FATAL( MENGINE_STRLEN( _path ) == 0 || (MENGINE_STRRCHR( _path, '.' ) > MENGINE_STRRCHR( _path, MENGINE_PATH_DELIM ) || _path[MENGINE_STRLEN( _path ) - 1] == '/') );

        MENGINE_ASSERTION_FATAL( MENGINE_STRLEN( _directory ) == 0 || (MENGINE_STRRCHR( _directory, '.' ) > MENGINE_STRRCHR( _directory, MENGINE_PATH_DELIM ) || _directory[MENGINE_STRLEN( _directory ) - 1] == '/') );

        WChar unicode_path[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar unicode_directory[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _directory, unicode_directory, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        bool result = this->createDirectory_( unicode_path, unicode_directory );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::createDirectory_( const WChar * _path, const WChar * _directory )
    {
        size_t unicode_directorySize = MENGINE_WCSLEN( _directory );

        if( unicode_directorySize == 0 )
        {
            return true;
        }

        WChar pathDirectory[MENGINE_MAX_PATH] = {L'\0'};
        Helper::pathCorrectBackslashToW( pathDirectory, _directory );

        Helper::pathRemoveFileSpecW( pathDirectory );

        if( MENGINE_WCSLEN( pathDirectory ) == 0 )
        {
            return true;
        }

        WChar pathTestDirectory[MENGINE_MAX_PATH] = {L'\0'};
        MENGINE_WNSPRINTF( pathTestDirectory, MENGINE_MAX_PATH, L"%s%s", _path, pathDirectory );

        if( ::PathIsDirectoryW( pathTestDirectory ) == FILE_ATTRIBUTE_DIRECTORY )
        {
            return true;
        }

        Helper::pathRemoveBackslashW( pathDirectory );

        VectorWString paths;

        for( ;; )
        {
            paths.emplace_back( pathDirectory );

            if( Helper::pathRemoveFileSpecW( pathDirectory ) == false )
            {
                break;
            }

            Helper::pathRemoveBackslashW( pathDirectory );

            MENGINE_WNSPRINTF( pathTestDirectory, MENGINE_MAX_PATH, L"%s%s", _path, pathDirectory );

            if( ::PathIsDirectoryW( pathTestDirectory ) == FILE_ATTRIBUTE_DIRECTORY )
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

            const WChar * path_str = path.c_str();

            WChar pathCreateDirectory[MENGINE_MAX_PATH] = {L'\0'};
            MENGINE_WNSPRINTF( pathCreateDirectory, MENGINE_MAX_PATH, L"%s%s", _path, path_str );

            BOOL successful = ::CreateDirectory( pathCreateDirectory, NULL );

            if( successful == FALSE )
            {
                DWORD error = ::GetLastError();

                switch( error )
                {
                case ERROR_ALREADY_EXISTS:
                    {
                        continue;
                    }break;
                case ERROR_PATH_NOT_FOUND:
                    {
                        LOGGER_ERROR( "directory '%ls' not found"
                            , pathCreateDirectory
                        );

                        return false;
                    }break;
                default:
                    {
                        LOGGER_ERROR( "directory '%ls' unknown %s"
                            , pathCreateDirectory
                            , Helper::Win32GetLastErrorMessage()
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
            LOGGER_ERROR( "invalid DeleteFile '%ls' %s"
                , pathCorrect
                , Helper::Win32GetLastErrorMessage()
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
                LOGGER_ERROR( "invalid move file '%ls' %s"
                    , newFilePathCorrect
                    , Helper::Win32GetLastErrorMessage()
                );
            }
        }

        if( ::MoveFile( oldFilePathCorrect, newFilePathCorrect ) == FALSE )
        {
            LOGGER_ERROR( "file '%ls' move to '%ls' %s"
                , oldFilePathCorrect
                , newFilePathCorrect
                , Helper::Win32GetLastErrorMessage()
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
                HANDLE hFind = ::FindFirstFileEx( sPath, FindExInfoStandard, &fdFile, FindExSearchNameMatch, NULL, 0 );

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

        if( handle == INVALID_HANDLE_VALUE )
        {
            LOGGER_ERROR( "get file time '%ls' invalid CreateFile %s"
                , unicode_filePath
                , Helper::Win32GetLastErrorMessage()
            );

            return 0U;
        }

        FILETIME creation;
        FILETIME access;
        FILETIME write;

        if( ::GetFileTime( handle, &creation, &access, &write ) == FALSE )
        {
            ::CloseHandle( handle );

            LOGGER_ERROR( "get file time '%ls' invalid GetFileTime %s"
                , unicode_filePath
                , Helper::Win32GetLastErrorMessage()
            );

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
    bool Win32Platform::createDirectoryUser_( const WChar * _userPath, const WChar * _directoryPath, const WChar * _file, const void * _data, size_t _size )
    {
        WChar szPath[MENGINE_MAX_PATH] = {L'\0'};
        Helper::pathCorrectBackslashToW( szPath, _userPath );

        WChar pathCorrect[MENGINE_MAX_PATH] = {L'\0'};
        Helper::pathCorrectBackslashToW( pathCorrect, _directoryPath );

        WChar fileCorrect[MENGINE_MAX_PATH] = {L'\0'};
        Helper::pathCorrectBackslashToW( fileCorrect, _file );

        MENGINE_WCSCAT( szPath, pathCorrect );

        if( this->existFile_( szPath ) == false )
        {
            if( this->createDirectory_( _userPath, _directoryPath ) == false )
            {
                LOGGER_ERROR( "directory '%ls:%ls' invalid createDirectory '%ls'"
                    , pathCorrect
                    , fileCorrect
                    , szPath
                );

                return false;
            }
        }

        MENGINE_WCSCAT( szPath, fileCorrect );

        HANDLE hFile = ::CreateFile( szPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

        if( hFile == INVALID_HANDLE_VALUE )
        {
            LOGGER_ERROR( "'%ls:%ls' invalid createFile '%ls' %s"
                , pathCorrect
                , fileCorrect
                , szPath
                , Helper::Win32GetLastErrorMessage()
            );

            return false;
        }

        DWORD bytesWritten = 0;
        BOOL result = ::WriteFile( hFile, _data, (DWORD)_size, &bytesWritten, NULL );

        ::CloseHandle( hFile );

        if( result == FALSE )
        {
            LOGGER_ERROR( "'%ls:%ls' invalid writeFile '%ls'"
                , pathCorrect
                , fileCorrect
                , szPath
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::getSpecialFolderPath_( DWORD _flag, WChar * const _path ) const
    {
        HRESULT hr = ::SHGetFolderPath( NULL, _flag, NULL, 0, _path );

        if( hr != S_OK )
        {
            LOGGER_ERROR( "invalid SHGetFolderPath flag [%lu] error [hr %lu]"
                , _flag
                , hr
            );

            return false;
        }

        Helper::pathCorrectBackslashW( _path );
        MENGINE_WCSCAT( _path, L"/" );

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

        WChar szPath[MENGINE_MAX_PATH] = {L'\0'};
        if( this->getSpecialFolderPath_( CSIDL_COMMON_PICTURES | CSIDL_FLAG_CREATE, szPath ) == false )
        {
            LOGGER_ERROR( "invalid getSpecialFolderPath CSIDL_COMMON_PICTURES '%s%s'"
                , _directoryPath
                , _filePath
            );

            return false;
        }

        WChar unicode_directoryPath_correct[MENGINE_MAX_PATH] = {L'\0'};
        Helper::pathCorrectBackslashToW( unicode_directoryPath_correct, unicode_directoryPath );

        MENGINE_WCSCAT( szPath, unicode_directoryPath_correct );

        WChar unicode_filePath_correct[MENGINE_MAX_PATH] = {L'\0'};
        Helper::pathCorrectBackslashToW( unicode_filePath_correct, unicode_filePath );

        MENGINE_WCSCAT( szPath, unicode_filePath_correct );

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

        WChar szPath[MENGINE_MAX_PATH] = {L'\0'};
        if( this->getSpecialFolderPath_( CSIDL_COMMON_PICTURES | CSIDL_FLAG_CREATE, szPath ) == false )
        {
            LOGGER_ERROR( "invalid SHGetFolderPath CSIDL_COMMON_PICTURES '%s%s'"
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

        WChar szPath[MENGINE_MAX_PATH] = {L'\0'};
        if( this->getSpecialFolderPath_( CSIDL_COMMON_MUSIC | CSIDL_FLAG_CREATE, szPath ) == false )
        {
            LOGGER_ERROR( "invalid SHGetFolderPath CSIDL_COMMON_MUSIC '%s%s'"
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
#if defined MENGINE_PLATFORM_WINDOWS
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        // dbghelp.dll
        //////////////////////////////////////////////////////////////////////////
        typedef VOID( __stdcall * TRtlCaptureContext )(OUT PCONTEXT ContextRecord);
        typedef BOOL( __stdcall * TSymCleanup )(IN HANDLE hProcess);
        typedef PVOID( __stdcall * TSymFunctionTableAccess64 )(HANDLE hProcess, DWORD64 AddrBase);
        typedef BOOL( __stdcall * TSymGetLineFromAddr64 )(IN HANDLE hProcess, IN DWORD64 dwAddr, OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE64 Line);
        typedef DWORD64( __stdcall * TSymGetModuleBase64 )(IN HANDLE hProcess, IN DWORD64 dwAddr);
        typedef BOOL( __stdcall * TSymGetModuleInfo64 )(IN HANDLE hProcess, IN DWORD64 dwAddr, OUT PIMAGEHLP_MODULE64 ModuleInfo);
        typedef DWORD( __stdcall * TSymGetOptions )(VOID);
        typedef BOOL( __stdcall * TSymGetSymFromAddr64 )(IN HANDLE hProcess, IN DWORD64 dwAddr, OUT PDWORD64 pdwDisplacement, OUT PIMAGEHLP_SYMBOL64 Symbol);
        typedef BOOL( __stdcall * TSymInitialize )(IN HANDLE hProcess, IN PSTR UserSearchPath, IN BOOL fInvadeProcess);
        typedef DWORD64( __stdcall * TSymLoadModule64 )(IN HANDLE hProcess, IN HANDLE hFile, IN PCSTR ImageName, IN PCSTR ModuleName, IN DWORD64 BaseOfDll, IN DWORD SizeOfDll);
        typedef DWORD( __stdcall * TSymSetOptions )(IN DWORD SymOptions);
        typedef BOOL( __stdcall * TStackWalk64 )(
            DWORD MachineType,
            HANDLE hProcess,
            HANDLE hThread,
            LPSTACKFRAME64 StackFrame,
            PVOID ContextRecord,
            PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
            PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
            PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,
            PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress);
        typedef DWORD( __stdcall WINAPI * TUnDecorateSymbolName )(PCSTR DecoratedName, PSTR UnDecoratedName, DWORD UndecoratedLength, DWORD Flags);
        typedef BOOL( __stdcall WINAPI * TSymGetSearchPath )(HANDLE hProcess, PSTR SearchPath, DWORD SearchPathLength);
        //////////////////////////////////////////////////////////////////////////
        // psapi.dll
        //////////////////////////////////////////////////////////////////////////
        typedef BOOL( __stdcall * TEnumProcessModules )(HANDLE hProcess, HMODULE * lphModule, DWORD cb, LPDWORD lpcbNeeded);
        typedef DWORD( __stdcall * TGetModuleFileNameEx )(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize);
        typedef DWORD( __stdcall * TGetModuleBaseName )(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize);
        typedef BOOL( __stdcall * TGetModuleInformation )(HANDLE hProcess, HMODULE hModule, LPMODULEINFO pmi, DWORD nSize);
        //////////////////////////////////////////////////////////////////////////
        static BOOL GetModuleListPSAPI( TSymLoadModule64 pSymLoadModule64, HANDLE hProcess )
        {
            HINSTANCE hPsapi = ::LoadLibraryW( L"psapi.dll" );

            if( hPsapi == NULL )
            {
                LOGGER_ERROR( "invalid open psapi.dll" );

                return FALSE;
            }

            TEnumProcessModules pEnumProcessModules = (TEnumProcessModules)::GetProcAddress( hPsapi, "EnumProcessModules" );
            TGetModuleFileNameEx pGetModuleFileNameExA = (TGetModuleFileNameEx)::GetProcAddress( hPsapi, "GetModuleFileNameExA" );
            TGetModuleBaseName pGetModuleBaseNameA = (TGetModuleFileNameEx)::GetProcAddress( hPsapi, "GetModuleBaseNameA" );
            TGetModuleInformation pGetModuleInformation = (TGetModuleInformation)::GetProcAddress( hPsapi, "GetModuleInformation" );

            if( (pEnumProcessModules == NULL) || (pGetModuleFileNameExA == NULL) || (pGetModuleBaseNameA == NULL) || (pGetModuleInformation == NULL) )
            {
                LOGGER_ERROR( "invalid load function psapi.dll" );

                ::FreeLibrary( hPsapi );

                return FALSE;
            }

            HMODULE hMods[2048];

            DWORD cbNeeded;
            if( (*pEnumProcessModules)(hProcess, hMods, sizeof( hMods ), &cbNeeded) == FALSE )
            {
                DWORD le = ::GetLastError();

                LOGGER_ERROR( "invalid enum process modules psapi.dll [%lu]"
                    , le
                );

                ::FreeLibrary( hPsapi );

                return FALSE;
            }

            //cppcheck-suppress uninitvar
            if( cbNeeded > 2048 )
            {
                LOGGER_ERROR( "invalid needed > 2048 psapi.dll" );

                ::FreeLibrary( hPsapi );

                return FALSE;
            }

            for( DWORD i = 0; i != cbNeeded / sizeof( HMODULE ); i++ )
            {
                MODULEINFO mi;
                if( (*pGetModuleInformation)(hProcess, hMods[i], &mi, sizeof mi) == FALSE )
                {
                    LOGGER_ERROR( "invalid get module information psapi.dll" );

                    ::FreeLibrary( hPsapi );

                    return FALSE;
                }

                Char mFileName[MAX_PATH] = {'\0'};
                DWORD FileNameLen = (*pGetModuleFileNameExA)(hProcess, hMods[i], mFileName, MAX_PATH);

                Char mBaseName[MAX_PATH] = {'\0'};
                DWORD BaseNameLen = (*pGetModuleBaseNameA)(hProcess, hMods[i], mBaseName, MAX_PATH);

                if( FileNameLen <= 0 || BaseNameLen <= 0 )
                {
                    LOGGER_ERROR( "invalid file base name len psapi.dll" );

                    ::FreeLibrary( hPsapi );

                    return FALSE;
                }

                if( pSymLoadModule64( hProcess, 0, mFileName, mBaseName, (DWORD64)mi.lpBaseOfDll, mi.SizeOfImage ) == FALSE )
                {
                    LOGGER_ERROR( "invalid load module file '%s' base '%s' psapi.dll"
                        , mFileName
                        , mBaseName
                    );

                    ::FreeLibrary( hPsapi );

                    return FALSE;
                }
            }

            ::FreeLibrary( hPsapi );

            return TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static BOOL GetModuleInfo( TSymGetModuleInfo64 pSymGetModuleInfo64, HANDLE hProcess, DWORD64 baseAddr, PIMAGEHLP_MODULE64 pModuleInfo )
        {
            if( pSymGetModuleInfo64 == NULL )
            {
                return FALSE;
            }

            pModuleInfo->SizeOfStruct = sizeof( IMAGEHLP_MODULE64 );

            uint8_t pData[4096]; // reserve enough memory, so the bug in v6.3.5.1 does not lead to memory-overwrites...
            MENGINE_MEMCPY( pData, pModuleInfo, sizeof( IMAGEHLP_MODULE64 ) );

            if( pSymGetModuleInfo64( hProcess, baseAddr, (IMAGEHLP_MODULE64 *)pData ) == FALSE )
            {
                return FALSE;
            }

            MENGINE_MEMCPY( pModuleInfo, pData, sizeof( IMAGEHLP_MODULE64 ) );
            pModuleInfo->SizeOfStruct = sizeof( IMAGEHLP_MODULE64 );

            return TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static BOOL __stdcall ReadMemoryRoutine( HANDLE hProcess, DWORD64 qwBaseAddress, PVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead )
        {
            SIZE_T st;
            BOOL bRet = ::ReadProcessMemory( hProcess, (LPVOID)qwBaseAddress, lpBuffer, nSize, &st );

            *lpNumberOfBytesRead = (DWORD)st;

            return bRet;
        }
        //////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_STACKWALK_MAX_NAMELEN
#define MENGINE_STACKWALK_MAX_NAMELEN 1024
#endif
        //////////////////////////////////////////////////////////////////////////
        typedef struct _CallstackEntry
        {
            DWORD64 offset;
            CHAR name[MENGINE_STACKWALK_MAX_NAMELEN];
            CHAR undName[MENGINE_STACKWALK_MAX_NAMELEN];
            CHAR undFullName[MENGINE_STACKWALK_MAX_NAMELEN];
            DWORD lineNumber;
            CHAR lineFileName[MENGINE_STACKWALK_MAX_NAMELEN];
            CHAR moduleName[MENGINE_STACKWALK_MAX_NAMELEN];
        } CallstackEntry;
        //////////////////////////////////////////////////////////////////////////
        enum CallstackEntryType
        {
            firstEntry, nextEntry, lastEntry
        };
        //////////////////////////////////////////////////////////////////////////
        static size_t OnCallstackEntry( Char * const _stack, size_t _capacity, CallstackEntry & entry )
        {
            CHAR buffer[MENGINE_STACKWALK_MAX_NAMELEN];
            if( entry.offset == 0 )
            {
                return 0;
            }

            if( entry.name[0] == 0 )
            {
                MENGINE_STRCPY( entry.name, "(function-name not available)" );
            }

            if( entry.undName[0] != 0 )
            {
                MENGINE_STRCPY( entry.name, entry.undName );
            }

            if( entry.undFullName[0] != 0 )
            {
                MENGINE_STRCPY( entry.name, entry.undFullName );
            }

            if( entry.lineFileName[0] == 0 )
            {
                MENGINE_STRCPY( entry.lineFileName, "(filename not available)" );

                if( entry.moduleName[0] == 0 )
                {
                    MENGINE_STRCPY( entry.moduleName, "(module-name not available)" );
                }

                MENGINE_SPRINTF( buffer, "%p (%s): %s (%d): %s\n"
                    , (LPVOID)entry.offset
                    , entry.moduleName
                    , entry.lineFileName
                    , entry.lineNumber
                    , entry.name
                );
            }
            else
            {
                MENGINE_SPRINTF( buffer, "%s (%d): %s\n"
                    , entry.lineFileName
                    , entry.lineNumber
                    , entry.name
                );
            }

            size_t buffer_len = MENGINE_STRLEN( buffer );

            if( _capacity < buffer_len )
            {
                return 0;
            }

            MENGINE_STRCAT( _stack, buffer );

            return buffer_len;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool GetCallstack( Char * const _stack, size_t _capacity, PCONTEXT _context, HMODULE hDbhHelp, HMODULE hKernel32, HANDLE hThread, HANDLE hProcess )
        {
            MENGINE_ASSERTION_FATAL( _capacity != 0 );

            _stack[0] = '\0';
            --_capacity;

            TRtlCaptureContext pRtlCaptureContext = (TRtlCaptureContext)::GetProcAddress( hKernel32, "RtlCaptureContext" );
            TStackWalk64 pStackWalk64 = (TStackWalk64)::GetProcAddress( hDbhHelp, "StackWalk64" );
            TSymGetOptions pSymGetOptions = (TSymGetOptions)::GetProcAddress( hDbhHelp, "SymGetOptions" );
            TSymSetOptions pSymSetOptions = (TSymSetOptions)::GetProcAddress( hDbhHelp, "SymSetOptions" );

            TSymFunctionTableAccess64 pSymFunctionTableAccess64 = (TSymFunctionTableAccess64)::GetProcAddress( hDbhHelp, "SymFunctionTableAccess64" );
            TSymGetLineFromAddr64 pSymGetLineFromAddr64 = (TSymGetLineFromAddr64)::GetProcAddress( hDbhHelp, "SymGetLineFromAddr64" );
            TSymGetModuleBase64 pSymGetModuleBase64 = (TSymGetModuleBase64)::GetProcAddress( hDbhHelp, "SymGetModuleBase64" );
            TSymGetModuleInfo64 pSymGetModuleInfo64 = (TSymGetModuleInfo64)::GetProcAddress( hDbhHelp, "SymGetModuleInfo64" );
            TSymGetSymFromAddr64 pSymGetSymFromAddr64 = (TSymGetSymFromAddr64)::GetProcAddress( hDbhHelp, "SymGetSymFromAddr64" );
            TUnDecorateSymbolName pUnDecorateSymbolName = (TUnDecorateSymbolName)::GetProcAddress( hDbhHelp, "UnDecorateSymbolName" );
            TSymLoadModule64 pSymLoadModule64 = (TSymLoadModule64)::GetProcAddress( hDbhHelp, "SymLoadModule64" );
            TSymGetSearchPath pSymGetSearchPath = (TSymGetSearchPath)::GetProcAddress( hDbhHelp, "SymGetSearchPath" );

            if( pRtlCaptureContext == NULL || pStackWalk64 == NULL || pSymGetOptions == NULL ||
                pSymSetOptions == NULL || pSymFunctionTableAccess64 == NULL || pSymGetModuleBase64 == NULL ||
                pSymGetModuleInfo64 == NULL || pSymGetSymFromAddr64 == NULL || pUnDecorateSymbolName == NULL || pSymLoadModule64 == NULL ||
                pSymGetSearchPath == NULL )
            {
                LOGGER_ERROR( "invalid get function" );

                return false;
            }

            DWORD symOptions = pSymGetOptions();
            symOptions |= SYMOPT_LOAD_LINES;
            symOptions |= SYMOPT_FAIL_CRITICAL_ERRORS;

            symOptions = (*pSymSetOptions)(symOptions);

            if( Detail::GetModuleListPSAPI( pSymLoadModule64, hProcess ) == FALSE )
            {
                LOGGER_ERROR( "invalid GetModuleListPSAPI" );

                return false;
            }

            CONTEXT contex;

            if( _context == NULL )
            {
                MENGINE_MEMSET( &contex, 0, sizeof( CONTEXT ) );
                contex.ContextFlags = CONTEXT_FULL;

                HANDLE hCurrentThread = ::GetCurrentThread();

                if( hThread == hCurrentThread )
                {
                    (*pRtlCaptureContext)(&contex);
                }
                else
                {
                    if( ::SuspendThread( hThread ) == (DWORD)-1 )
                    {
                        return FALSE;
                    }

                    if( ::GetThreadContext( hThread, &contex ) == FALSE )
                    {
                        LOGGER_ERROR( "invalid get thread context %s"
                            , Helper::Win32GetLastErrorMessage()
                        );

                        ::ResumeThread( hThread );

                        return FALSE;
                    }
                }
            }
            else
            {
                contex = *_context;
            }

            STACKFRAME64 frame;
            MENGINE_MEMSET( &frame, 0, sizeof( frame ) );

#ifdef _M_IX86
            DWORD imageType = IMAGE_FILE_MACHINE_I386;
            frame.AddrPC.Offset = contex.Eip;
            frame.AddrPC.Mode = AddrModeFlat;
            frame.AddrFrame.Offset = contex.Ebp;
            frame.AddrFrame.Mode = AddrModeFlat;
            frame.AddrStack.Offset = contex.Esp;
            frame.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
            DWORD imageType = IMAGE_FILE_MACHINE_AMD64;
            frame.AddrPC.Offset = contex.Rip;
            frame.AddrPC.Mode = AddrModeFlat;
            frame.AddrFrame.Offset = contex.Rsp;
            frame.AddrFrame.Mode = AddrModeFlat;
            frame.AddrStack.Offset = contex.Rsp;
            frame.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
            DWORD imageType = IMAGE_FILE_MACHINE_IA64;
            frame.AddrPC.Offset = contex.StIIP;
            frame.AddrPC.Mode = AddrModeFlat;
            frame.AddrFrame.Offset = contex.IntSp;
            frame.AddrFrame.Mode = AddrModeFlat;
            frame.AddrBStore.Offset = contex.RsBSP;
            frame.AddrBStore.Mode = AddrModeFlat;
            frame.AddrStack.Offset = contex.IntSp;
            frame.AddrStack.Mode = AddrModeFlat;
#else
#error "Platform not supported!"
#endif

            uint8_t pSymBuff[sizeof( IMAGEHLP_SYMBOL64 ) + MENGINE_STACKWALK_MAX_NAMELEN];
            IMAGEHLP_SYMBOL64 * pSym = (IMAGEHLP_SYMBOL64 *)pSymBuff;
            MENGINE_MEMSET( pSym, 0, sizeof( IMAGEHLP_SYMBOL64 ) + MENGINE_STACKWALK_MAX_NAMELEN );

            pSym->SizeOfStruct = sizeof( IMAGEHLP_SYMBOL64 );
            pSym->MaxNameLength = MENGINE_STACKWALK_MAX_NAMELEN;

            IMAGEHLP_LINE64 Line;
            MENGINE_MEMSET( &Line, 0, sizeof( Line ) );
            Line.SizeOfStruct = sizeof( Line );

            IMAGEHLP_MODULE64 Module;
            MENGINE_MEMSET( &Module, 0, sizeof( Module ) );
            Module.SizeOfStruct = sizeof( Module );

            uint32_t recursionFrame = 0;

            for( int32_t frameNum = 0;; ++frameNum )
            {
                if( (*pStackWalk64)(imageType, hProcess, hThread, &frame, &contex, &ReadMemoryRoutine, pSymFunctionTableAccess64, pSymGetModuleBase64, NULL) == FALSE )
                {
                    LOGGER_ERROR( "invalid pStackWalk64" );

                    if( _context == NULL )
                    {
                        ::ResumeThread( hThread );
                    }

                    return false;
                }

                if( frame.AddrPC.Offset == frame.AddrReturn.Offset )
                {
                    if( ++recursionFrame == 1024 )
                    {
                        break;
                    }
                }

                CallstackEntry csEntry;
                MENGINE_MEMSET( &csEntry, 0, sizeof( csEntry ) );
                csEntry.offset = frame.AddrPC.Offset;

                if( frame.AddrPC.Offset != 0 )
                {
                    DWORD64 offsetFromSmybol;
                    if( (*pSymGetSymFromAddr64)(hProcess, frame.AddrPC.Offset, &offsetFromSmybol, pSym) == TRUE )
                    {
                        MENGINE_STRCPY( csEntry.name, pSym->Name );
                        (*pUnDecorateSymbolName)(pSym->Name, csEntry.undName, MENGINE_STACKWALK_MAX_NAMELEN, UNDNAME_NAME_ONLY);
                        (*pUnDecorateSymbolName)(pSym->Name, csEntry.undFullName, MENGINE_STACKWALK_MAX_NAMELEN, UNDNAME_COMPLETE);
                    }

                    if( pSymGetLineFromAddr64 != NULL )
                    {
                        DWORD offsetFromLine;
                        if( (*pSymGetLineFromAddr64)(hProcess, frame.AddrPC.Offset, &offsetFromLine, &Line) == TRUE )
                        {
                            csEntry.lineNumber = Line.LineNumber;
                            MENGINE_STRCPY( csEntry.lineFileName, Line.FileName );
                        }
                    }

                    if( Detail::GetModuleInfo( pSymGetModuleInfo64, hProcess, frame.AddrPC.Offset, &Module ) == TRUE )
                    {
                        MENGINE_STRCPY( csEntry.moduleName, Module.ModuleName );
                    }
                }

                size_t stack_len = Detail::OnCallstackEntry( _stack, _capacity, csEntry );

                _capacity -= stack_len;

                if( frame.AddrReturn.Offset == 0 )
                {
                    break;
                }
            }

            if( _context == NULL )
            {
                ::ResumeThread( hThread );
            }

            return true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::getCallstack( uint64_t _threadId, Char * const _stack, size_t _capacity, PCONTEXT _context ) const
    {
        HANDLE hProcess = ::GetCurrentProcess();

        HMODULE hDbhHelp = ::LoadLibraryW( L"dbghelp.dll" );

        if( hDbhHelp == NULL )
        {
            LOGGER_ERROR( "invalid load 'dbghelp.dll'" );

            return false;
        }

        Detail::TSymInitialize pSymInitialize = (Detail::TSymInitialize)::GetProcAddress( hDbhHelp, "SymInitialize" );
        Detail::TSymCleanup pSymCleanup = (Detail::TSymCleanup)::GetProcAddress( hDbhHelp, "SymCleanup" );

        if( pSymInitialize == NULL || pSymCleanup == NULL )
        {
            LOGGER_ERROR( "invalid get dbghelp.dll [SymInitialize||SymCleanup]" );

            ::FreeLibrary( hDbhHelp );

            return false;
        }

        HMODULE hKernel32 = ::LoadLibraryW( L"Kernel32.dll" );

        if( hKernel32 == NULL )
        {
            ::FreeLibrary( hDbhHelp );

            LOGGER_ERROR( "invalid load Kernel32.dll" );

            return false;
        }

        if( (*pSymInitialize)(hProcess, NULL, FALSE) == FALSE )
        {
            LOGGER_ERROR( "invalid SymInitialize" );

            ::FreeLibrary( hDbhHelp );
            ::FreeLibrary( hKernel32 );

            return false;
        }

        HANDLE hThread;

        if( _threadId == ~0U )
        {
            hThread = ::GetCurrentThread();
        }
        else
        {
            hThread = ::OpenThread( THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_QUERY_INFORMATION, FALSE, (DWORD)_threadId );
        }

        if( hThread == NULL )
        {
            LOGGER_ERROR( "invalid open thread [%llu] ShowWindow %s"
                , _threadId
                , Helper::Win32GetLastErrorMessage()
            );


            LOGGER_ERROR( "invalid load 'dbghelp.dll'" );

            ::FreeLibrary( hDbhHelp );
            ::FreeLibrary( hKernel32 );

            return false;
        }

        bool successful = Detail::GetCallstack( _stack, _capacity, (PCONTEXT)_context, hDbhHelp, hKernel32, hThread, hProcess );

        (*pSymCleanup)(hProcess);

        if( _threadId != ~0U )
        {
            ::CloseHandle( hThread );
        }

        ::FreeLibrary( hDbhHelp );
        ::FreeLibrary( hKernel32 );

        return successful;
    }
#else
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::getCallstack( uint64_t _threadId, Char * const _stack, size_t _capacity, PCONTEXT _context ) const
    {
        MENGINE_UNUSED( _threadId );
        MENGINE_UNUSED( _stack );
        MENGINE_UNUSED( _capacity );
        MENGINE_UNUSED( _context );

        LOGGER_ERROR( "this platform not support callstack" );

        return true;
    }
#endif
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::isNeedWindowRender() const
    {
        if( ::IsIconic( m_hWnd ) == TRUE )
        {
            return false;
        }

        bool nopause = APPLICATION_SERVICE()
            ->getNopause();

        if( nopause == true )
        {
            return true;
        }

        bool focus = APPLICATION_SERVICE()
            ->isFocus();

        if( focus == true && m_active == true )
        {
            return true;
        }

        return false;
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
        Helper::utf8ToUnicode( _command, unicode_command + 1, 4096 );

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
                LOGGER_ERROR( "CreateProcess '%s' return %s"
                    , _process
                    , Helper::Win32GetLastErrorMessage()
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
                DWORD nNumberOfBytesToRead = MENGINE_MIN( tempFileSize, 4095 );
                BOOL successful = ::ReadFile( hReadTempFile, tempFileBuffer, nNumberOfBytesToRead, &dwBytesRead, NULL );

                if( successful == TRUE )
                {
                    LOGGER_VERBOSE_LEVEL( ConstString::none(), LM_ERROR, LCOLOR_RED, nullptr, 0 )("%s"
                        , tempFileBuffer
                        );
                }
                else
                {
                    LOGGER_VERBOSE_LEVEL( ConstString::none(), LM_ERROR, LCOLOR_RED, nullptr, 0 )("invalid read file '%ls'"
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
                LOGGER_ERROR( "invalid CreateProcess '%s' %s"
                    , _process
                    , Helper::Win32GetLastErrorMessage()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    DynamicLibraryInterfacePtr Win32Platform::loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentPtr & _doc )
    {
        LOGGER_INFO( "platform", "load dynamic library '%s'"
            , _dynamicLibraryName
        );

        Win32DynamicLibraryPtr dynamicLibrary = m_factoryDynamicLibraries->createObject( _doc );

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

        if( cxscreen == 0 )
        {
            LOGGER_ERROR( "GetSystemMetrics SM_CXSCREEN invalid %s"
                , Helper::Win32GetLastErrorMessage()
            );

            return false;
        }

        int32_t cyscreen = ::GetSystemMetrics( SM_CYSCREEN );

        if( cyscreen == 0 )
        {
            LOGGER_ERROR( "GetSystemMetrics SM_CYSCREEN invalid %s"
                , Helper::Win32GetLastErrorMessage()
            );

            return false;
        }

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
        const Char * option_workdir;
        if( HAS_OPTION_VALUE( "workdir", &option_workdir ) == true )
        {
            MENGINE_ASSERTION_FATAL( MENGINE_STRLEN( option_workdir ) < MENGINE_MAX_PATH );

            MENGINE_STRCPY( _currentPath, option_workdir );

            Helper::pathCorrectBackslashA( _currentPath );

            size_t option_workdir_len = MENGINE_STRLEN( _currentPath );

            return option_workdir_len;
        }

        WChar currentPath[MENGINE_MAX_PATH] = {L'\0'};
        DWORD len = ::GetCurrentDirectory( MENGINE_MAX_PATH, currentPath );

        if( len == 0 )
        {
            LOGGER_ERROR( "GetCurrentDirectory invalid %s"
                , Helper::Win32GetLastErrorMessage()
            );

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
        bool developmentMode = Helper::isDevelopmentMode();
        bool OPTION_roamingMode = HAS_OPTION( "roaming" );
        bool OPTION_noroamingMode = HAS_OPTION( "noroaming" );

        if( (developmentMode == true && OPTION_roamingMode == false) || OPTION_noroamingMode == true )
        {
            WChar currentPath[MENGINE_MAX_PATH] = {L'\0'};
            DWORD len = ::GetCurrentDirectory( MENGINE_MAX_PATH, currentPath );

            if( len == 0 )
            {
                LOGGER_ERROR( "failed to get current directory" );

                return 0;
            }

            Helper::pathCorrectBackslashW( currentPath );

            ::PathRemoveBackslash( currentPath );

            MENGINE_WCSCAT( currentPath, L"/" );
            MENGINE_WCSCAT( currentPath, MENGINE_DEVELOPMENT_USER_FOLDER_NAME );

            uint32_t Engine_BotId = CONFIG_VALUE( "Engine", "BotId", ~0U );

            if( Engine_BotId != ~0U || HAS_OPTION( "bot" ) == true )
            {
                uint32_t botId = GET_OPTION_VALUE_UINT32( "bot", Engine_BotId );

                WChar botId_suffix[16];
                MENGINE_WSPRINTF( botId_suffix, L"%u", botId );

                MENGINE_WCSCAT( currentPath, botId_suffix );
            }

            MENGINE_WCSCAT( currentPath, L"/" );

            size_t currentPathLen;
            if( Helper::unicodeToUtf8( currentPath, _userPath, MENGINE_MAX_PATH, &currentPathLen ) == false )
            {
                LOGGER_ERROR( "invalid convert path from unicode to utf8 '%ls'"
                    , currentPath
                );

                return 0;
            }

            return currentPathLen;
        }

        LPITEMIDLIST itemIDList;

        HRESULT hr = ::SHGetSpecialFolderLocation( NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, &itemIDList );

        if( hr != S_OK )
        {
            Char str_hr[4096] = {'\0'};
            this->getErrorMessage( hr, str_hr, 4095 );

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
            LOGGER_ERROR( "SHGetPathFromIDListW invalid" );

            return 0;
        }

        ::CoTaskMemFree( itemIDList );

        const Char * companyName = CONFIG_VALUE( "Project", "Company", "NONAME" );

        if( MENGINE_STRLEN( companyName ) == 0 )
        {
            LOGGER_ERROR( "invalid get company name" );

            return 0;
        }

        WChar companyNameW[MENGINE_APPLICATION_COMPANY_MAXNAME] = {L'\0'};
        if( Helper::utf8ToUnicode( companyName, companyNameW, MENGINE_APPLICATION_COMPANY_MAXNAME ) == false )
        {
            LOGGER_ERROR( "invalid convert company name from utf8 to unicode '%s'"
                , companyName
            );

            return 0;
        }

        WChar roamingPath[MENGINE_MAX_PATH] = {L'\0'};
        ::PathCombine( roamingPath, currentPath, companyNameW );

        const Char * projectName = CONFIG_VALUE( "Project", "Name", "UNKNOWN" );

        if( MENGINE_STRLEN( projectName ) == 0 )
        {
            LOGGER_ERROR( "invalid get project name" );

            return 0;
        }

        WChar projectNameW[MENGINE_APPLICATION_PROJECT_MAXNAME] = {L'\0'};
        if( Helper::utf8ToUnicode( projectName, projectNameW, MENGINE_APPLICATION_PROJECT_MAXNAME ) == false )
        {
            LOGGER_ERROR( "invalid convert project name from utf8 to unicode '%s'"
                , projectName
            );

            return 0;
        }

        ::PathCombine( roamingPath, roamingPath, projectNameW );

        MENGINE_WCSCAT( roamingPath, L"/" );

        size_t currentPathLen;
        if( Helper::unicodeToUtf8( roamingPath, _userPath, MENGINE_MAX_PATH, &currentPathLen ) == false )
        {
            LOGGER_ERROR( "invalid convert user path from unicode to utf8 '%ls'"
                , roamingPath
            );

            return 0;
        }

        return currentPathLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32Platform::getUserName( Char * const _userName ) const
    {
        WChar unicode_userName[UNLEN + 1] = {L'\0'};
        DWORD unicode_userNameLen = UNLEN + 1;
        if( ::GetUserName( unicode_userName, &unicode_userNameLen ) == FALSE )
        {
            LOGGER_ERROR( "GetUserName invalid %s"
                , Helper::Win32GetLastErrorMessage()
            );

            return 0;
        }

        size_t userNameLen;
        if( Helper::unicodeToUtf8Size( unicode_userName, unicode_userNameLen, _userName, MENGINE_PLATFORM_USER_MAXNAME, &userNameLen ) == false )
        {
            return 0;
        }

        return userNameLen;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::closeWindow()
    {
        m_close = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::minimizeWindow()
    {
        if( ::ShowWindow( m_hWnd, SW_MINIMIZE ) == FALSE )
        {
            LOGGER_ERROR( "invalid hwnd [%p] ShowWindow %s"
                , m_hWnd
                , Helper::Win32GetLastErrorMessage()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::setCursorPosition( const mt::vec2f & _pos )
    {
        POINT cPos;
        cPos.x = (int32_t)_pos.x;
        cPos.y = (int32_t)_pos.y;

        if( ::ClientToScreen( m_hWnd, &cPos ) == FALSE )
        {
            return;
        }

        ::SetCursorPos( cPos.x, cPos.y );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::setCursorIcon( const ConstString & _icon )
    {
        MapCursors::const_iterator it_found = m_cursors.find( _icon );

        MENGINE_ASSERTION_FATAL( it_found != m_cursors.end() );

        const CursorDesc & desc = it_found->second;

        m_cursor = desc.cursor;

        ::SetCursor( m_cursor );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::hasCursorIcon( const ConstString & _icon ) const
    {
        MapCursors::const_iterator it_found = m_cursors.find( _icon );

        if( it_found == m_cursors.end() )
        {
            return false;
        }

        return true;
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
        WChar unicode_url[4096] = {L'\0'};
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

        mt::vec2f point( 0.f, 0.f );
        this->calcCursorPosition_( &point );

        if( m_active == false )
        {
            if( nopause == false )
            {
                Helper::pushMouseLeaveEvent( TC_TOUCH0, point.x, point.y, 0.f );
            }
        }
        else
        {
            Helper::pushMouseEnterEvent( TC_TOUCH0, point.x, point.y, 0.f );
        }

        bool focus = m_active;

        if( nopause == true )
        {
            focus = true;
        }

        APPLICATION_SERVICE()
            ->setFocus( focus );

        INPUT_SERVICE()
            ->onFocus( focus );

        APPLICATION_SERVICE()
            ->turnSound( focus );

        if( m_active == true )
        {
            if( m_cursor == NULL )
            {
                m_cursor = ::LoadCursor( NULL, IDC_ARROW );
            }

            ::SetCursor( m_cursor );
        }
        else
        {
            ::SetCursor( NULL );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::messageBox( const Char * _caption, const Char * _format, ... ) const
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        Char str[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
        int32_t size_vsnprintf = MENGINE_VSNPRINTF( str, MENGINE_LOGGER_MAX_MESSAGE, _format, args );
        MENGINE_VA_LIST_END( args );

        if( size_vsnprintf < 0 )
        {
            LOGGER_ERROR( "invalid message box format message '%s'"
                , _format
            );

            ::MessageBoxA( NULL, "invalid message box format message", _caption, MB_OK );

            return;
        }

        ::MessageBoxA( NULL, str, _caption, MB_OK );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::setClipboardText( const Char * _value ) const
    {
        size_t len = MENGINE_STRLEN( _value );

        HGLOBAL hGlb = ::GlobalAlloc( GMEM_MOVEABLE, len + 1 );

        LPVOID memGlb = ::GlobalLock( hGlb );

        MENGINE_MEMCPY( memGlb, _value, len );

        ::GlobalUnlock( memGlb );

        if( ::OpenClipboard( m_hWnd ) == FALSE )
        {
            ::GlobalFree( hGlb );

            return false;
        }

        ::EmptyClipboard();

        ::SetClipboardData( CF_TEXT, hGlb );

        ::CloseClipboard();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::getClipboardText( Char * _value, size_t _capacity ) const
    {
        if( ::OpenClipboard( m_hWnd ) == FALSE )
        {
            return false;
        }

        HANDLE hGlb = ::GetClipboardData( CF_TEXT );

        if( hGlb == NULL )
        {
            ::CloseClipboard();

            return false;
        }

        LPVOID memGlb = ::GlobalLock( hGlb );

        if( memGlb == NULL )
        {
            ::CloseClipboard();

            return false;
        }

        const Char * clipboardText = (const Char *)memGlb;

        MENGINE_STRNCPY( _value, clipboardText, _capacity );

        ::GlobalUnlock( hGlb );

        ::CloseClipboard();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Win32Platform::addWin32ProcessHandler( const LambdaWin32ProcessHandler & _lambda, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        UniqueId id = ENUMERATOR_SERVICE()
            ->generateUniqueIdentity();

        Win32ProcessDesc desc;
        desc.id = id;
        desc.lambda = _lambda;

#if MENGINE_DOCUMENT_ENABLE
        desc.doc = _doc;
#endif

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
    //////////////////////////////////////////////////////////////////////////
    bool Win32Platform::initializeFileService_()
    {
        LOGGER_INFO( "system", "Initialize Win32 file group..." );

        PLUGIN_CREATE( Win32FileGroup, MENGINE_DOCUMENT_FACTORABLE );

        Char currentPath[MENGINE_MAX_PATH] = {'\0'};
        size_t currentPathLen = PLATFORM_SERVICE()
            ->getCurrentPath( currentPath );

        if( currentPathLen == 0 )
        {
            LOGGER_ERROR( "failed to get current directory" );

            return false;
        }

        LOGGER_MESSAGE( "Current Path: %s"
            , currentPath
        );

        if( FILE_SERVICE()
            ->mountFileGroup( ConstString::none(), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "dir" ), nullptr, false, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "failed to mount application directory: '%s'"
                , currentPath
            );

            return false;
        }

#ifndef MENGINE_MASTER_RELEASE
        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "global" ), nullptr, false, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "failed to mount dev directory: '%s'"
                , currentPath
            );

            return false;
        }
#endif

        WChar winDir[MENGINE_MAX_PATH] = {L'\0'};
        UINT winDirLen = ::GetWindowsDirectory( winDir, MENGINE_MAX_PATH );

        if( winDirLen == 0 )
        {
            LOGGER_ERROR( "failed GetWindowsDirectory %s"
                , Helper::Win32GetLastErrorMessage()
            );
        }

        if( winDirLen >= MENGINE_MAX_PATH )
        {
            LOGGER_ERROR( "failed GetWindowsDirectory len %u great capacity %u"
                , winDirLen
                , MENGINE_MAX_PATH
            );

            return false;
        }

        Helper::pathCorrectBackslashW( winDir );
        ::PathRemoveBackslash( winDir );
        MENGINE_WCSCAT( winDir, L"/" );

        Utf8 utf8_winDir[MENGINE_MAX_PATH] = {'\0'};
        Helper::unicodeToUtf8( winDir, utf8_winDir, MENGINE_MAX_PATH );

        FilePath winDirPath = Helper::stringizeFilePath( utf8_winDir );

        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "windows" ), nullptr, nullptr, winDirPath, STRINGIZE_STRING_LOCAL( "global" ), nullptr, false, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "failed to mount dev directory: '%s'"
                , currentPath
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::finalizeFileService_()
    {
        FILE_SERVICE()
            ->unmountFileGroup( STRINGIZE_STRING_LOCAL( "windows" ) );

#ifndef MENGINE_MASTER_RELEASE
        FILE_SERVICE()
            ->unmountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ) );
#endif

        FILE_SERVICE()
            ->unmountFileGroup( ConstString::none() );
    }
    //////////////////////////////////////////////////////////////////////////
}
