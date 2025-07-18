#include "Win32PlatformService.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/ProfilerSystemInterface.h"
#include "Interface/DateTimeSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/PluginInterface.h"
#include "Interface/EnvironmentServiceInterface.h"

#include "Environment/Windows/WindowsIncluder.h"
#include "Environment/Windows/Win32Helper.h"
#include "Environment/Windows/Win32FileHelper.h"

#include "Win32CPUInfo.h"
#include "Win32DynamicLibrary.h"

#if defined(MENGINE_PLUGIN_DEVTODEBUG)
#   include "Plugins/DevToDebugPlugin/DevToDebugInterface.h"

#   include "Win32PlatformDevToDebug.h"
#endif

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/Stringalized.h"
#include "Kernel/StringHelper.h"
#include "Kernel/BuildMode.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/SHA1.h"
#include "Kernel/Stringstream.h"
#include "Kernel/RandomDevice.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/ExecutorHelper.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/Utf8Helper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/Hexadecimal.h"
#include "Kernel/VocabularyHelper.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"
#include "Config/StdIntTypes.h"
#include "Config/Utf8.h"
#include "Config/StdAlgorithm.h"
#include "Config/Path.h"

#include <clocale>
#include <ctime>
#include <functional>
#include <cerrno>

typedef HRESULT( WINAPI * FGetDpiForMonitor )(HMONITOR, MONITOR_DPI_TYPE, UINT *, UINT *);

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_SETLOCALE
#define MENGINE_SETLOCALE 1
#endif
//////////////////////////////////////////////////////////////////////////
#if MENGINE_SETLOCALE == 1
#   define MENGINE_SETLOCALE_ENABLE
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_SETLOCALE_VALUE
#define MENGINE_SETLOCALE_VALUE "C"
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
#ifndef MENGINE_WINDOW_ICON_CACHE
#define MENGINE_WINDOW_ICON_CACHE "icon_cache"
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PlatformService, Mengine::Win32PlatformService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32PlatformService::Win32PlatformService()
        : m_beginTime( 0 )
        , m_hInstance( NULL )
        , m_hWnd( NULL )
        , m_xDpi( ~0U )
        , m_yDpi( ~0U )
        , m_performanceFrequency{0}
        , m_performanceSupport( false )
        , m_active( false )
        , m_hIcon( NULL )
        , m_close( false )
        , m_sleepMode( true )
        , m_windowExposed( false )
        , m_pauseUpdatingTime( -1.f )
        , m_prevTime( 0.0 )
        , m_cursorInArea( false )
        , m_cursorMode( false )
        , m_cursor( NULL )
        , m_lastMouse( false )
        , m_lastMouseX( 0 )
        , m_lastMouseY( 0 )
        , m_touchpad( false )
        , m_desktop( false )
        , m_fullscreen( false )
#if defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
        , m_sessionNotification( false )
        , m_sessionLock( false )
#endif
    {
        StdAlgorithm::fill_n( m_clickOutArea, MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE, false );
        StdAlgorithm::fill_n( m_isDoubleClick, MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE, false );
    }
    //////////////////////////////////////////////////////////////////////////
    Win32PlatformService::~Win32PlatformService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::_initializeService()
    {
        m_beginTime = Helper::getSystemTimestamp();

#if defined(MENGINE_SETLOCALE_ENABLE)
        ::setlocale( LC_ALL, MENGINE_SETLOCALE_VALUE );
#endif

        m_hInstance = ::GetModuleHandle( NULL );

        if( ::QueryPerformanceFrequency( &m_performanceFrequency ) == TRUE )
        {
            m_performanceSupport = true;
        }

        m_cursors[STRINGIZE_STRING_LOCAL( "IDC_ARROW" )] = {::LoadCursor( NULL, IDC_ARROW ), true};
        m_cursors[STRINGIZE_STRING_LOCAL( "IDC_UPARROW" )] = {::LoadCursor( NULL, IDC_UPARROW ), true};
        m_cursors[STRINGIZE_STRING_LOCAL( "IDC_HAND" )] = {::LoadCursor( NULL, IDC_HAND ), true};
        m_cursors[STRINGIZE_STRING_LOCAL( "IDC_HELP" )] = {::LoadCursor( NULL, IDC_HELP ), true};

        const Char * option_platforms[MENGINE_OPTIONS_VALUES_MAX];
        uint32_t option_platforms_count;
        if( OPTIONS_SERVICE()
            ->getOptionValues( "platform", option_platforms, &option_platforms_count ) == true )
        {
            for( uint32_t index = 0; index != option_platforms_count; ++index )
            {
                const Char * option_platform = option_platforms[index];

                Char uppercase_option_platform[256 + 1] = {'\0'};
                Helper::toupper( option_platform, uppercase_option_platform, 255 );

                m_platformTags.addTag( Helper::stringizeString( option_platform ) );
            }
        }
        else if( HAS_OPTION( "win32" ) == true )
        {
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN32" ) );

            m_touchpad = false;
            m_desktop = true;
        }
        else if( HAS_OPTION( "win64" ) == true )
        {
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN64" ) );

            m_touchpad = false;
            m_desktop = true;
        }
        else if( HAS_OPTION( "mac" ) == true )
        {
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "MAC" ) );

#if defined(MENGINE_PLATFORM_WINDOWS)
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
#endif

#if defined(MENGINE_PLATFORM_WINDOWS32)
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN32" ) );
#endif

#if defined(MENGINE_PLATFORM_WINDOWS64)
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN64" ) );
#endif

            m_touchpad = false;
            m_desktop = true;
        }
        else if( HAS_OPTION( "ios" ) == true )
        {
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "IOS" ) );

#if defined(MENGINE_PLATFORM_WINDOWS)
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
#endif

#if defined(MENGINE_PLATFORM_WINDOWS32)
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN32" ) );
#endif

#if defined(MENGINE_PLATFORM_WINDOWS64)
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN64" ) );
#endif

            m_touchpad = true;
            m_desktop = false;
        }
        else if( HAS_OPTION( "android" ) == true )
        {
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "ANDROID" ) );

#if defined(MENGINE_PLATFORM_WINDOWS)
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
#endif

#if defined(MENGINE_PLATFORM_WINDOWS32)
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN32" ) );
#endif

#if defined(MENGINE_PLATFORM_WINDOWS64)
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN64" ) );
#endif

            m_touchpad = true;
            m_desktop = false;
        }
        else if( HAS_OPTION( "wp" ) == true )
        {
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WP" ) );

            m_touchpad = true;
            m_desktop = false;
        }
        else
        {
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );

#if defined(MENGINE_PLATFORM_WINDOWS32)
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN32" ) );
#endif

#if defined(MENGINE_PLATFORM_WINDOWS64)
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN64" ) );
#endif

            m_touchpad = false;
            m_desktop = true;
        }

        if( HAS_OPTION( "table" ) == true )
        {
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "TABLET" ) );
        }
        else if( HAS_OPTION( "phone" ) == true )
        {
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PHONE" ) );
        }

        Char platformTagsInfo[1024 + 1] = {'\0'};

        for( const ConstString & tag : m_platformTags.getValues() )
        {
            StdString::strcat_safe( platformTagsInfo, tag.c_str(), 1024 );
            StdString::strcat_safe( platformTagsInfo, "-", 1024 );
        }

        LOGGER_INFO( "platform", "platform tags: %s"
            , platformTagsInfo
        );

        if( HAS_OPTION( "touchpad" ) == true )
        {
            m_touchpad = true;
        }

        LOGGER_INFO( "platform", "touchpad: %u"
            , m_touchpad
        );

        if( HAS_OPTION( "desktop" ) == true )
        {
            m_desktop = true;
        }

        LOGGER_INFO( "platform", "desktop: %u"
            , m_desktop
        );

        m_factoryDynamicLibraries = Helper::makeFactoryPool<Win32DynamicLibrary, 8>( MENGINE_DOCUMENT_FACTORABLE );

        const Char * Window_ClassName = CONFIG_VALUE_STRING( "Window", "ClassName", MENGINE_WINDOW_CLASSNAME );

        Helper::utf8ToUnicode( Window_ClassName, m_windowClassName.data(), MENGINE_MAX_PATH );

#if defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
        if( HAS_OPTION( "workdir" ) == true )
        {
            Path currentPath = {'\0'};
            if( this->getCurrentPath( currentPath ) == 0 )
            {
                LOGGER_ERROR( "invalid get current path for dll directory" );

                return false;
            }

            WPath currentPathW = {L'\0'};
            if( Helper::utf8ToUnicode( currentPath, currentPathW, MENGINE_MAX_PATH, nullptr ) == false )
            {
                return false;
            }

            if( ::SetDllDirectoryW( currentPathW ) == FALSE )
            {
                LOGGER_ERROR( "SetDllDirectoryW [%ls] invalid %ls"
                    , currentPathW
                    , Helper::Win32GetLastErrorMessageW()
                );

                return false;
            }
        }
#endif

        uint64_t deviceSeed = Helper::generateRandomDeviceSeed();

        MENGINE_UNUSED( deviceSeed );

        LOGGER_INFO_PROTECTED( "platform", "device seed: %" MENGINE_PRIu64
            , deviceSeed
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::_finalizeService()
    {
        m_active = false;

        m_platformTags.clear();

        m_cursor = NULL;

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

#if defined(MENGINE_DEBUG)
        for( const Win32ProcessDesc & desc : m_win32ProcessHandlers )
        {
            MENGINE_UNUSED( desc );

            LOGGER_ASSERTION( "forgot remove win32 process handler (doc: %s)"
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#endif

        m_win32ProcessHandlers.clear();

        if( m_hIcon != NULL )
        {
            ::DestroyIcon( m_hIcon );
            m_hIcon = NULL;
        }

        this->detachWindow();

        if( m_hInstance != NULL )
        {
            LPCWSTR lpClassName = m_windowClassName.c_str();

            if( ::UnregisterClass( lpClassName, m_hInstance ) == FALSE )
            {
                LOGGER_ERROR( "invalid UnregisterClass [%ls] get error: %ls"
                    , m_windowClassName.c_str()
                    , Helper::Win32GetLastErrorMessageW()
                );
            }

            m_hInstance = NULL;
        }

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDynamicLibraries );

        m_factoryDynamicLibraries = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::_runService()
    {
        PlatformDateTime dateTime;
        DATETIME_SYSTEM()
            ->getLocalDateTime( &dateTime );

        LOGGER_INFO( "platform", "start date: %02u.%02u.%u, %02u:%02u:%02u"
            , dateTime.day
            , dateTime.month
            , dateTime.year
            , dateTime.hour
            , dateTime.minute
            , dateTime.second
        );

        SYSTEM_INFO sysInfo;
        ::GetSystemInfo( &sysInfo );

        LOGGER_INFO( "platform", "[Hardware information]" );
        LOGGER_INFO( "platform", "  OEM ID: %lu", sysInfo.dwOemId );
        LOGGER_INFO( "platform", "  Number of processors: %lu", sysInfo.dwNumberOfProcessors );
        LOGGER_INFO( "platform", "  Page size: %lu", sysInfo.dwPageSize );
        LOGGER_INFO( "platform", "  Processor type: %lu", sysInfo.dwProcessorType );
        LOGGER_INFO( "platform", "  Minimum application address: %p", sysInfo.lpMinimumApplicationAddress );
        LOGGER_INFO( "platform", "  Maximum application address: %p", sysInfo.lpMaximumApplicationAddress );
        LOGGER_INFO( "platform", "  Active processor mask: %" MENGINE_PRDWORD_PTR, sysInfo.dwActiveProcessorMask );

        LOGGER_INFO( "platform", "CPU information:" );

        Win32CPUInfo cpuinfo;

        LOGGER_INFO( "platform", "  Vendor: %s", cpuinfo.Vendor().c_str() );
        LOGGER_INFO( "platform", "  Brand: %s", cpuinfo.Brand().c_str() );

        auto support_message = []( const Char * isa_feature, bool is_supported )
        {
            uint32_t color = (is_supported == true ? LCOLOR_GREEN | LCOLOR_BLUE : LCOLOR_RED);

            LOGGER_VERBOSE_LEVEL( "platform", LM_INFO, LFILTER_NONE, color, nullptr, 0, nullptr, LFLAG_SHORT )("%s: %s"
                , isa_feature
                , is_supported == true ? "+" : "-"
                );
        };

        LOGGER_INFO( "platform", "CPU instruction:" );
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
            LOGGER_INFO( "platform", "start memory: %u.%uMb total, %u.%uMb free, %u.%uMb Page file total, %u.%uMb Page file free"
                , (uint32_t)(mem_st.ullTotalPhys / (1000UL * 1000UL) / 1000UL), (uint32_t)(mem_st.ullTotalPhys / (1000UL * 1000UL) % 1000UL)
                , (uint32_t)(mem_st.ullAvailPhys / (1000UL * 1000UL) / 1000UL), (uint32_t)(mem_st.ullAvailPhys / (1000UL * 1000UL) % 1000UL)
                , (uint32_t)(mem_st.ullTotalPageFile / (1000UL * 1000UL) / 1000UL), (uint32_t)(mem_st.ullTotalPageFile / (1000UL * 1000UL) % 1000UL)
                , (uint32_t)(mem_st.ullAvailPageFile / (1000UL * 1000UL) / 1000UL), (uint32_t)(mem_st.ullAvailPageFile / (1000UL * 1000UL) % 1000UL)
            );
        }

        if( this->setProcessDPIAware() == false )
        {
            LOGGER_INFO( "platform", "application not setup Process DPI Aware" );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::_stopService()
    {
        PlatformDateTime dateTime;
        DATETIME_SYSTEM()
            ->getLocalDateTime( &dateTime );

        LOGGER_INFO( "platform", "stop date: %02u.%02u.%u, %02u:%02u:%02u"
            , dateTime.day
            , dateTime.month
            , dateTime.year
            , dateTime.hour
            , dateTime.minute
            , dateTime.second
        );

        MEMORYSTATUSEX mem_st;
        mem_st.dwLength = sizeof( mem_st );

        if( ::GlobalMemoryStatusEx( &mem_st ) == TRUE )
        {
            LOGGER_INFO( "platform", "stop memory: %u.%uMb total, %u.%uMb free, %u.%uMb Page file total, %u.%uMb Page file free"
                , (uint32_t)(mem_st.ullTotalPhys / (1000UL * 1000UL) / 1000UL), (uint32_t)(mem_st.ullTotalPhys / (1000UL * 1000UL) % 1000UL)
                , (uint32_t)(mem_st.ullAvailPhys / (1000UL * 1000UL) / 1000UL), (uint32_t)(mem_st.ullAvailPhys / (1000UL * 1000UL) % 1000UL)
                , (uint32_t)(mem_st.ullTotalPageFile / (1000UL * 1000UL) / 1000UL), (uint32_t)(mem_st.ullTotalPageFile / (1000UL * 1000UL) % 1000UL)
                , (uint32_t)(mem_st.ullAvailPageFile / (1000UL * 1000UL) / 1000UL), (uint32_t)(mem_st.ullAvailPageFile / (1000UL * 1000UL) % 1000UL)
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::setProcessDPIAware()
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
    bool Win32PlatformService::isDebuggerPresent() const
    {
        if( ::IsDebuggerPresent() == FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::debugBreak()
    {
        ::DebugBreak();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::setSleepMode( bool _sleepMode )
    {
        m_sleepMode = _sleepMode;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::getSleepMode() const
    {
        return m_sleepMode;
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp Win32PlatformService::getPlatfomTime() const
    {
        Timestamp currentTime = Helper::getSystemTimestamp();

        Timestamp platformTime = currentTime - m_beginTime;

        return platformTime;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::runPlatform()
    {
        if( m_close == true )
        {
            return false;
        }

        this->setActive_( true );

        if( this->updatePlatform() == false )
        {
            return false;
        }

        this->tickPlatform( 0.f, false, false, false );

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_RUN );

        LOGGER_INFO( "platform", "run platform" );

        MENGINE_PROFILER_BEGIN_APPLICATION();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::updateWndMessage_()
    {
        MENGINE_PROFILER_CATEGORY();

        MSG msg;
        while( ::PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) == TRUE )
        {
            ::TranslateMessage( &msg );
            ::DispatchMessage( &msg );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::tickPlatform( float _frameTime, bool _render, bool _flush, bool _pause )
    {
        MENGINE_UNUSED( _frameTime );

#if defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
        if( m_sessionLock == true )
        {
            ::Sleep( 200 );

            return;
        }
#endif

        bool updating = APPLICATION_SERVICE()
            ->beginUpdate( _frameTime );

        if( updating == true )
        {
            if( m_pauseUpdatingTime >= 0.f )
            {
                _frameTime = m_pauseUpdatingTime;
                m_pauseUpdatingTime = -1.f;
            }

            APPLICATION_SERVICE()
                ->tick( _frameTime );
        }

        APPLICATION_SERVICE()
            ->endUpdate();

        if( this->isNeedWindowRender() == true && _render == true )
        {
            bool sucessful = APPLICATION_SERVICE()
                ->render();

            if( sucessful == true && _flush == true )
            {
                APPLICATION_SERVICE()
                    ->flush();
            }

            m_windowExposed = false;
        }

        if( _pause == true )
        {
            if( updating == false )
            {
                if( m_pauseUpdatingTime < 0.f )
                {
                    m_pauseUpdatingTime = _frameTime;
                }

                if( m_sleepMode == true )
                {
                    ::Sleep( 100 );
                }
                else
                {
                    ::Sleep( 1 );
                }
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
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::loopPlatform()
    {
        m_prevTime = Helper::getElapsedTime();

        while( m_close == false )
        {
            MENGINE_PROFILER_FRAME( "main" );

            NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_UPDATE );

            if( this->updatePlatform() == false )
            {
                break;
            }

            double currentTime = Helper::getElapsedTime();

            float frameTime = (float)(currentTime - m_prevTime);

            m_prevTime = currentTime;

            this->tickPlatform( frameTime, true, true, true );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::updatePlatform()
    {
        this->updateWndMessage_();

        if( m_close == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::stopPlatform()
    {
        LOGGER_INFO( "platform", "stop platform" );

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_STOP );

        if( m_alreadyRunningMonitor != nullptr )
        {
            m_alreadyRunningMonitor->finalize();
            m_alreadyRunningMonitor = nullptr;
        }

        m_mouseEvent.stop();

        this->detachWindow();

        MENGINE_PROFILER_END_APPLICATION();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::setHWNDIcon( const WChar * _iconResource )
    {
        HICON hIcon = ::LoadIcon( m_hInstance, _iconResource );

        if( hIcon == NULL )
        {
            LOGGER_ERROR( "invalid load HWND icon '%ls' %ls"
                , _iconResource
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        m_hIcon = hIcon;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::setProjectTitle( const Char * _projectTitle )
    {
        if( _projectTitle == nullptr )
        {
            m_projectTitle.clear();

            return;
        }

        Helper::utf8ToUnicodeSize( _projectTitle, MENGINE_UNKNOWN_SIZE, m_projectTitle.data(), MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME );

        LOGGER_INFO( "platform", "project title: %s"
            , _projectTitle
        );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32PlatformService::getProjectTitle( Char * const _projectTitle ) const
    {
        size_t utf8Size;
        Helper::unicodeToUtf8( m_projectTitle.c_str(), _projectTitle, MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME, &utf8Size );

        return utf8Size;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32PlatformService::getShortPathName( const Char * _path, Char * const _shortpath ) const
    {
        WPath unicode_path = {L'\0'};
        if( Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR( "invalid convert path to unicode" );

            return 0;
        }

        WPath unicode_shortpath = {L'\0'};
        DWORD len = ::GetShortPathName( unicode_path, unicode_shortpath, MENGINE_MAX_PATH );

        if( Helper::unicodeToUtf8Size( unicode_shortpath, (size_t)len, _shortpath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR( "invalid convert shortpath to utf8" );

            return 0;
        }

        return (size_t)len;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32PlatformService::getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const
    {
        WPath unicode_fontName = {L'\0'};
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
        result = ::RegOpenKeyExW( HKEY_LOCAL_MACHINE, fontRegistryPath, 0, KEY_READ, &hKey );

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

        WPath unicode_fontPath = {L'\0'};

        do
        {
            DWORD valueDataSize = maxValueDataSize;
            DWORD valueNameSize = maxValueNameSize;

            DWORD valueType;
            WPath valueName = {L'\0'};
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
                StdString::memcpy( unicode_fontPath, valueData, valueDataSize );

                break;
            }
        } while( result != ERROR_NO_MORE_ITEMS );

        ::RegCloseKey( hKey );

        if( StdString::wcslen( unicode_fontPath ) == 0 )
        {
            _fontPath[0] = '\0';

            return MENGINE_PATH_INVALID_LENGTH;
        }

        WPath fullDir = {L'\0'};
        MENGINE_SWPRINTF( fullDir, MENGINE_MAX_PATH, L"Fonts\\%ls"
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
    bool Win32PlatformService::getMaxClientResolution( Resolution * const _resolution ) const
    {
        RECT workArea;
        if( ::SystemParametersInfo( SPI_GETWORKAREA, 0, &workArea, 0 ) == FALSE )
        {
            LOGGER_ERROR( "invalid get system parameters info %ls"
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        DWORD dwStyle = this->getWindowStyle_( false );
        DWORD dwExStyle = this->getWindowExStyle_( false );

        RECT clientArea = workArea;

        if( ::AdjustWindowRectEx( &clientArea, dwStyle, FALSE, dwExStyle ) == FALSE )
        {
            LOGGER_ERROR( "invalid adjust window rect %ls"
                , Helper::Win32GetLastErrorMessageW()
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
    const Tags & Win32PlatformService::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::hasPlatformTag( const ConstString & _tag ) const
    {
        bool result = m_platformTags.hasTag( _tag );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::isDesktop() const
    {
        return m_desktop;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::hasTouchpad() const
    {
        return m_touchpad;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t Win32PlatformService::dpToWidthPx( int32_t _dp ) const
    {
        return ::MulDiv( _dp, m_xDpi, 96 );
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t Win32PlatformService::dpToHeightPx( int32_t _px ) const
    {
        return ::MulDiv( _px, m_yDpi, 96 );
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t Win32PlatformService::pxToWidthDp( int32_t _px ) const
    {
        return ::MulDiv( _px, 96, m_xDpi );
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t Win32PlatformService::pxToHeightDp( int32_t _px ) const
    {
        return ::MulDiv( _px, 96, m_yDpi );
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static LRESULT CALLBACK wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
        {
            switch( uMsg )
            {
                //case WM_NCCREATE:
            case WM_CREATE:
                {
                    LPCREATESTRUCTW createStruct = (LPCREATESTRUCTW)lParam;

                    Win32PlatformService * app = (Win32PlatformService *)createStruct->lpCreateParams;

#if defined(MENGINE_PLATFORM_WINDOWS64)
                    ::SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)app );
#else
                    ::SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG)app );
#endif

                    return (LRESULT)NULL;
                }
                break;
            }

            LONG_PTR value = ::GetWindowLongPtr( hWnd, GWLP_USERDATA );

            Win32PlatformService * platform = (Win32PlatformService *)value;

            if( platform == nullptr )
            {
                LRESULT result = ::DefWindowProc( hWnd, uMsg, wParam, lParam );

                return result;
            }

            LRESULT app_result = platform->wndProc( hWnd, uMsg, wParam, lParam );

            return app_result;
        }
        //////////////////////////////////////////////////////////////////////////
        static EMouseButtonCode getXButtonCode( DWORD lParam )
        {
            if( lParam == XBUTTON1 )
            {
                return MC_X1BUTTON;
            }
            else if( lParam == XBUTTON2 )
            {
                return MC_X2BUTTON;
            }

            return MC_X1BUTTON;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    LRESULT Win32PlatformService::wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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
                LOGGER_INFO( "platform", "HWND [%p] handled proccess wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d] (doc: %s)"
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
        case WM_SHOWWINDOW:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_SHOWWINDOW wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

            }break;
        case WM_ACTIVATE:
            {
                DWORD flagActive = LOWORD( wParam );
                BOOL minimized = (BOOL)HIWORD( wParam );

                LOGGER_INFO( "platform", "HWND [%p] WM_ACTIVATE active [%" MENGINE_PRDWORD "] minimized [%d] visible [%d]"
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
                LOGGER_INFO( "platform", "HWND [%p] WM_ACTIVATEAPP wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

            }break;
        case WM_SETFOCUS:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_SETFOCUS wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

            }break;
        case WM_KILLFOCUS:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_KILLFOCUS wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

            }break;
        case WM_ENTERIDLE:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_ENTERIDLE wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

            }break;
        case WM_PAINT:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_PAINT wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                RECT rect;
                if( ::GetUpdateRect( hWnd, &rect, FALSE ) )
                {
                    ::ValidateRect( hWnd, NULL );

                    m_windowExposed = true;
                }

            }break;
        case WM_ERASEBKGND:
            {
                //We'll do our own drawing, prevent flicker

                return 1;
            }break;
#if defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
        case WM_WTSSESSION_CHANGE:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_WTSSESSION_CHANGE wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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
                LOGGER_INFO( "platform", "HWND [%p] WM_DISPLAYCHANGE wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

            }break;
        case WM_SIZE:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_SIZE wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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
                LOGGER_INFO( "platform", "HWND [%p] WM_GETMINMAXINFO wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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
                LOGGER_INFO( "platform", "HWND [%p] WM_CLOSE wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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
                LOGGER_INFO( "platform", "HWND [%p] WM_SYSKEYDOWN wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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

                Helper::pushKeyEvent( point.x, point.y, 0.f, code, true, false );
            }break;
        case WM_SYSKEYUP:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_SYSKEYUP wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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

                Helper::pushKeyEvent( point.x, point.y, 0.f, code, false, false );
            }break;
        case WM_SYSCOMMAND:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_SYSCOMMAND wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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
                LOGGER_INFO( "platform", "HWND [%p] WM_SETCURSOR wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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

                        if( m_cursor == NULL )
                        {
                            LOGGER_ERROR( "LoadCursor [IDC_ARROW] get error %ls"
                                , Helper::Win32GetLastErrorMessageW()
                            );

                            return 0;
                        }
                    }

                    ::SetCursor( m_cursor );
                }

                return 0;
            }break;
        case WM_DESTROY:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_DESTROY wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                LOGGER_MESSAGE( "quit application" );

                m_close = true;

                ::PostQuitMessage( 0 );

                return 0;
            }break;
        default:
            {
                LRESULT input_result;
                if( this->wndProcInput( hWnd, uMsg, wParam, lParam, &input_result ) == true )
                {
                    return input_result;
                }

                LOGGER_INFO( "platform", "HWND [%p] UNKNOWN [%u] hex |0x%04X| wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
                    , hWnd
                    , uMsg
                    , uMsg
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );
            }break;
        }

        LRESULT result = ::DefWindowProc( hWnd, uMsg, wParam, lParam );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::wndProcInput( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT * const _result )
    {
        bool handle = false;

        switch( uMsg )
        {
        case WM_TIMER:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_TIMER wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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
                LOGGER_INFO( "platform", "HWND [%p] UWM_MOUSE_LEAVE wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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
                        m_clickOutArea[MC_LBUTTON] = true;
                    }

                    if( (::GetKeyState( VK_RBUTTON ) & 0x8000) != 0 )
                    {
                        m_clickOutArea[MC_RBUTTON] = true;
                    }

                    if( (::GetKeyState( VK_MBUTTON ) & 0x8000) != 0 )
                    {
                        m_clickOutArea[MC_MBUTTON] = true;
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
                LOGGER_INFO( "platform", "HWND [%p] WM_MOUSEMOVE wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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

                if( m_clickOutArea[MC_LBUTTON] == true )
                {
                    m_clickOutArea[MC_LBUTTON] = false;

                    if( (::GetKeyState( VK_LBUTTON ) & 0x8000) == 0 )
                    {
                        Helper::pushMouseButtonEvent( TC_TOUCH0, point.x, point.y, MC_LBUTTON, 0.f, false );
                    }
                }

                if( m_clickOutArea[MC_RBUTTON] == true )
                {
                    m_clickOutArea[MC_RBUTTON] = false;

                    if( (::GetKeyState( VK_RBUTTON ) & 0x8000) == 0 )
                    {
                        Helper::pushMouseButtonEvent( TC_TOUCH0, point.x, point.y, MC_RBUTTON, 0.f, false );
                    }
                }

                if( m_clickOutArea[MC_MBUTTON] == true )
                {
                    m_clickOutArea[MC_MBUTTON] = false;

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
                    LOGGER_ERROR( "invalid hwnd [%p] get client rect %ls"
                        , m_hWnd
                        , Helper::Win32GetLastErrorMessageW()
                    );

                    return false;
                }

                if( rect.right == rect.left || rect.bottom == rect.top )
                {
                    LOGGER_ERROR( "invalid hwnd [%p] zero client rect"
                        , m_hWnd
                    );

                    return false;
                }

                float width = static_cast<float>(rect.right - rect.left);
                float height = static_cast<float>(rect.bottom - rect.top);

                fdx /= width;
                fdy /= height;

                Helper::pushMouseMoveEvent( TC_TOUCH0, point.x, point.y, fdx, fdy, 0.f, 0.f );

                handle = true;
                *_result = 0;
            }break;
        case WM_MOUSEWHEEL:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_MOUSEWHEEL wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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

                Helper::pushMouseWheelEvent( point.x, point.y, 0.f, WC_CENTRAL, scroll );

                handle = true;
                *_result = 0;
            }break;
        case WM_LBUTTONDBLCLK:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_LBUTTONDBLCLK wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                m_isDoubleClick[MC_LBUTTON] = true;

                handle = true;
                *_result = 0;
            }break;
        case WM_RBUTTONDBLCLK:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_RBUTTONDBLCLK wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                m_isDoubleClick[MC_RBUTTON] = true;

                handle = true;
                *_result = 0;
            }break;
        case WM_MBUTTONDBLCLK:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_MBUTTONDBLCLK wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                m_isDoubleClick[MC_MBUTTON] = true;

                handle = true;
                *_result = 0;
            }break;
        case WM_XBUTTONDBLCLK:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_XBUTTONDBLCLK wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                DWORD XButton = GET_XBUTTON_WPARAM( wParam );

                EMouseButtonCode code = Detail::getXButtonCode( XButton );

                m_isDoubleClick[code] = true;

                handle = true;
                *_result = 0;
            }break;
        case WM_LBUTTONDOWN:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_LBUTTONDOWN wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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
                LOGGER_INFO( "platform", "HWND [%p] WM_LBUTTONUP wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                if( m_isDoubleClick[MC_LBUTTON] == false )
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

                m_isDoubleClick[MC_LBUTTON] = false;

                handle = true;
                *_result = 0;
            }break;
        case WM_RBUTTONDOWN:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_RBUTTONDOWN wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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
                LOGGER_INFO( "platform", "HWND [%p] WM_RBUTTONUP wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                if( m_isDoubleClick[MC_RBUTTON] == false )
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

                m_isDoubleClick[MC_RBUTTON] = false;

                handle = true;
                *_result = 0;
            }break;
        case WM_MBUTTONDOWN:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_MBUTTONDOWN wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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
                LOGGER_INFO( "platform", "HWND [%p] WM_MBUTTONUP wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                if( m_isDoubleClick[MC_MBUTTON] == false )
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

                m_isDoubleClick[MC_MBUTTON] = false;

                handle = true;
                *_result = 0;
            }break;
        case WM_XBUTTONDOWN:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_XBUTTONDOWN wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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

                DWORD XButton = GET_XBUTTON_WPARAM( wParam );

                EMouseButtonCode code = Detail::getXButtonCode( XButton );

                Helper::pushMouseButtonEvent( TC_TOUCH0, point.x, point.y, code, 0.f, true );

                handle = true;
                *_result = 0;
            }break;
        case WM_XBUTTONUP:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_XBUTTONUP wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                DWORD XButton = GET_XBUTTON_WPARAM( wParam );

                EMouseButtonCode code = Detail::getXButtonCode( XButton );

                if( m_isDoubleClick[code] == false )
                {
                    POINT p;
                    p.x = GET_X_LPARAM( lParam );
                    p.y = GET_Y_LPARAM( lParam );

                    mt::vec2f point;
                    if( this->getCursorPosition_( p, &point ) == false )
                    {
                        return false;
                    }

                    Helper::pushMouseButtonEvent( TC_TOUCH0, point.x, point.y, code, 0.f, false );
                }

                m_isDoubleClick[code] = false;

                handle = true;
                *_result = 0;
            }break;
        case WM_KEYDOWN:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_KEYDOWN wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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

                Helper::pushKeyEvent( point.x, point.y, 0.f, code, true, false );

                handle = true;
                *_result = 0;
            }break;
        case WM_KEYUP:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_KEYUP wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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

                Helper::pushKeyEvent( point.x, point.y, 0.f, code, false, false );

                handle = true;
                *_result = 0;
            }break;
        case WM_UNICHAR:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_UNICHAR wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
                    , hWnd
                    , wParam
                    , lParam
                    , ::IsWindowVisible( hWnd )
                );

                if( wParam == UNICODE_NOCHAR )
                {
                    handle = true;
                    *_result = TRUE;

                    break;
                }

                if( this->sendChar_( wParam ) == false )
                {
                    return false;
                }

                handle = true;
                *_result = TRUE;
            }break;
        case WM_CHAR:
            {
                LOGGER_INFO( "platform", "HWND [%p] WM_CHAR wParam [%" MENGINE_PRWPARAM "] lParam [%" MENGINE_PRLPARAM "] visible [%d]"
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
    bool Win32PlatformService::sendChar_( WPARAM wParam )
    {
        Char utf8_code[5 + 1] = {'\0'};
        if( Helper::Utf32ToUtf8( (Utf32)wParam, utf8_code ) == false )
        {
            return false;
        }

        mt::vec2f point;
        if( this->calcCursorPosition_( &point ) == false )
        {
            return false;
        }

        WChar unicode_text[MENGINE_INPUTTEXTEVENT_TEXT_MAX_SIZE + 1] = {L'\0'};
        if( Helper::utf8ToUnicode( utf8_code, unicode_text, 8 ) == false )
        {
            return false;
        }

        Helper::pushTextEvent( point.x, point.y, 0.f, unicode_text );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::alreadyRunningMonitor()
    {
        bool Platform_AlreadyRunning = CONFIG_VALUE_BOOLEAN( "Platform", "AlreadyRunning", true );

        if( Platform_AlreadyRunning == true && HAS_OPTION( "noalreadyrunning" ) == false )
        {
            m_alreadyRunningMonitor = Helper::makeFactorableUnique<Win32AlreadyRunningMonitor>( MENGINE_DOCUMENT_FACTORABLE );

            const WChar * windowClassName_str = m_windowClassName.c_str();
            const WChar * projectTitle_str = m_projectTitle.c_str();

            bool stop;
            if( m_alreadyRunningMonitor->initialize( EARP_SETFOCUS, windowClassName_str, projectTitle_str, &stop ) == false )
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
    bool Win32PlatformService::createWindow( const Resolution & _resolution, bool _fullscreen )
    {
        m_windowResolution = _resolution;
        m_fullscreen = _fullscreen;

        bool Platform_WithoutWindow = CONFIG_VALUE_BOOLEAN( "Platform", "WithoutWindow", false );

        if( Platform_WithoutWindow == true || HAS_OPTION( "norender" ) == true )
        {
            LOGGER_INFO( "platform", "platform without window" );

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
        wc.lpszClassName = m_windowClassName.c_str();
        wc.hIconSm = NULL;

        ATOM result = ::RegisterClassEx( &wc );

        if( result == 0 )
        {
            LOGGER_ERROR( "can't register window class %ls"
                , Helper::Win32GetLastErrorMessageW()
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

        HWND hWnd = ::CreateWindowEx( dwExStyle, m_windowClassName.c_str(), m_projectTitle.c_str()
            , dwStyle
            , rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top
            , NULL, NULL, m_hInstance, (LPVOID)this );

        if( hWnd == NULL )
        {
            LOGGER_ERROR( "can't create window %ls"
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        if( this->atachWindow( hWnd, _fullscreen ) == false )
        {
            LOGGER_ERROR( "can't atach window" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::atachWindow( HWND _hWnd, bool _fullscreen )
    {
        MENGINE_ASSERTION_FATAL( _hWnd != NULL, "atachWindow hWnd is NULL" );

        m_hWnd = _hWnd;

        HWND hWndFgnd = ::GetForegroundWindow();

        if( hWndFgnd != m_hWnd )
        {
            LOGGER_INFO( "platform", "setup foreground window..." );

            ::ShowWindow( m_hWnd, SW_MINIMIZE );
            ::ShowWindow( m_hWnd, SW_RESTORE );
        }
        else
        {
            ::ShowWindow( m_hWnd, SW_SHOW );
        }

#if defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
        if( ::WTSRegisterSessionNotification( m_hWnd, NOTIFY_FOR_ALL_SESSIONS ) == FALSE )
        {
            LOGGER_ERROR( "invalid hwnd [%p] register session notification %ls"
                , m_hWnd
                , Helper::Win32GetLastErrorMessageW()
            );

            m_sessionNotification = false;
        }
        else
        {
            m_sessionNotification = true;
        }
#endif

        if( _fullscreen == true )
        {
            Resolution desktopResolution;
            this->getDesktopResolution( &desktopResolution );

            m_windowResolution = desktopResolution;
        }
        else
        {
            RECT rect;
            ::GetClientRect( _hWnd, &rect );

            m_windowResolution.setWidth( rect.right - rect.left );
            m_windowResolution.setHeight( rect.bottom - rect.top );
        }

        ::SetForegroundWindow( m_hWnd );
        ::SetFocus( m_hWnd );
        ::UpdateWindow( m_hWnd );

        HINSTANCE shcoreModule = ::LoadLibraryA( "shcore.dll" );

        m_xDpi = ~0U;
        m_yDpi = ~0U;

        if( shcoreModule != NULL )
        {
            FGetDpiForMonitor GetDpiForMonitor = (FGetDpiForMonitor)::GetProcAddress( shcoreModule, "GetDpiForMonitor" );
            if( GetDpiForMonitor != NULL )
            {
                HMONITOR monitor = ::MonitorFromWindow( (HWND)m_hWnd, MONITOR_DEFAULTTONEAREST );

                UINT xDpi = ~0U;
                UINT yDpi = ~0U;
                HRESULT hr = GetDpiForMonitor( monitor, MDT_EFFECTIVE_DPI, &xDpi, &yDpi );

                if( hr != S_OK )
                {
                    LOGGER_ERROR( "invalid hwnd [%p] get dpi for monitor %ls"
                        , m_hWnd
                        , Helper::Win32GetErrorMessageW( hr )
                    );

                    return false;
                }

                m_xDpi = xDpi;
                m_yDpi = yDpi;
            }

            ::FreeLibrary( shcoreModule );
        }

#ifndef NOGDI
        if( m_xDpi == ~0U || m_yDpi == ~0U )
        {
            HDC dc = ::GetDC( m_hWnd );
            UINT xDpi = ::GetDeviceCaps( dc, LOGPIXELSX );
            UINT yDpi = ::GetDeviceCaps( dc, LOGPIXELSY );
            ::ReleaseDC( m_hWnd, dc );

            m_xDpi = xDpi;
            m_yDpi = yDpi;
        }
#endif

        m_mouseEvent.setHWND( m_hWnd );

        this->updateWndMessage_();

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_ATACH_WINDOW );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::detachWindow()
    {
        if( m_hWnd == NULL )
        {
            return false;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_DETACH_WINDOW );

#if defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
        if( m_sessionNotification == true )
        {
            m_sessionNotification = false;

            if( ::WTSUnRegisterSessionNotification( m_hWnd ) == FALSE )
            {
                LOGGER_ERROR( "invalid hwnd [%p] unregister session notification %ls"
                    , m_hWnd
                    , Helper::Win32GetLastErrorMessageW()
                );
            }
        }
#endif

        ::CloseWindow( m_hWnd );
        ::DestroyWindow( m_hWnd );

        m_hWnd = NULL;

        this->updateWndMessage_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    HWND Win32PlatformService::getWindowHandle() const
    {
        return m_hWnd;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
        LOGGER_INFO( "platform", "window mode changed resolution %u:%u fullscreen [%u]"
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
    void Win32PlatformService::notifyVsyncChanged( bool _vsync )
    {
        MENGINE_UNUSED( _vsync );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::notifyCursorModeChanged( bool _cursorMode )
    {
        if( m_cursorMode == _cursorMode )
        {
            return;
        }

        m_cursorMode = _cursorMode;

        if( m_cursorMode == true )
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
    HCURSOR Win32PlatformService::loadCursorICO_( const FilePath & _filePath, const MemoryInterfacePtr & _buffer ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _buffer, "invalid load cursor ICO '%s' buffer is NULL"
            , _filePath.c_str()
        );

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
        icoFile += MENGINE_WINDOW_ICON_CACHE;
        icoFile += MENGINE_PATH_FORWARDSLASH;
        icoFile += _filePath;
        icoFile += ".ico";

        FilePath c_icoFile = Helper::stringizeFilePath( icoFile );

        if( fileGroup->createDirectory( c_icoFile ) == false )
        {
            LOGGER_ERROR( "invalid create directory '.icon_cache'" );

            return NULL;
        }

        if( Helper::writeOutputStreamFile( fileGroup, c_icoFile, true, _buffer, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "path '%s' can't write ico '%s'"
                , _filePath.c_str()
                , c_icoFile.c_str()
            );

            return NULL;
        }

        Path icoFullFile = {L'\0'};
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
            LOGGER_ERROR( "icon file '%ls' %ls"
                , unicode_icoFile_str
                , Helper::Win32GetLastErrorMessageW()
            );

            return NULL;
        }

        return cursor;
    }
    //////////////////////////////////////////////////////////////////////////
    HCURSOR Win32PlatformService::getCursorICO_( const ConstString & _name, const FilePath & _filePath, const MemoryInterfacePtr & _buffer )
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
    bool Win32PlatformService::notifyCursorIconSetup( const ConstString & _name, const ContentInterfacePtr & _content, const MemoryInterfacePtr & _buffer )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _content, "invalid setup cursor icon '%s' content is NULL"
            , _name.c_str()
        );

        const FilePath & filePath = _content->getFilePath();

        HCURSOR cursor = this->getCursorICO_( _name, filePath, _buffer );

        m_cursor = cursor;

        if( m_cursorMode == true )
        {
            ::SetCursor( m_cursor );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    DWORD Win32PlatformService::getWindowStyle_( bool _fullsreen ) const
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
    DWORD Win32PlatformService::getWindowExStyle_( bool _fullsreen ) const
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
    bool Win32PlatformService::calcWindowsRect_( const Resolution & _resolution, bool _fullsreen, RECT * const _rect ) const
    {
        uint32_t resolutionWidth = _resolution.getWidth();
        uint32_t resolutionHeight = _resolution.getHeight();

        RECT rc;
        if( ::SetRect( &rc, 0, 0, (int32_t)resolutionWidth, (int32_t)resolutionHeight ) == FALSE )
        {
            LOGGER_ERROR( "invalid set rect %ls"
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        if( _fullsreen == false )
        {
            uint32_t OPTION_winx = GET_OPTION_VALUE_UINT32( "winx", MENGINE_UNKNOWN_SIZE );
            uint32_t OPTION_winy = GET_OPTION_VALUE_UINT32( "winy", MENGINE_UNKNOWN_SIZE );

            if( OPTION_winx != MENGINE_UNKNOWN_SIZE && OPTION_winy != MENGINE_UNKNOWN_SIZE )
            {
                uint32_t width = rc.right - rc.left;
                uint32_t height = rc.bottom - rc.top;

                rc.left = OPTION_winx;
                rc.top = OPTION_winy;
                rc.right = rc.left + width;
                rc.bottom = rc.top + height;
            }
            else
            {
                DWORD dwStyle = this->getWindowStyle_( _fullsreen );
                DWORD dwStyleEx = this->getWindowExStyle_( _fullsreen );

                if( ::AdjustWindowRectEx( &rc, dwStyle, FALSE, dwStyleEx ) == FALSE )
                {
                    LOGGER_ERROR( "invalid adjust window rect %ls"
                        , Helper::Win32GetLastErrorMessageW()
                    );

                    return false;
                }

                RECT workArea;
                if( ::SystemParametersInfo( SPI_GETWORKAREA, 0, &workArea, 0 ) == FALSE )
                {
                    LOGGER_ERROR( "invalid system parameters info %ls"
                        , Helper::Win32GetLastErrorMessageW()
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
        }

        *_rect = rc;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::calcCursorPosition_( mt::vec2f * const _position ) const
    {
        POINT point;
        if( ::GetCursorPos( &point ) == FALSE )
        {
            LOGGER_ERROR( "invalid get cursor pos %ls"
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        bool result = this->adaptCursorPosition_( point, _position );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::adaptCursorPosition_( POINT _point, mt::vec2f * const _position ) const
    {
        POINT cPos = _point;
        if( ::ScreenToClient( m_hWnd, &cPos ) == FALSE )
        {
            LOGGER_ERROR( "invalid hwnd [%p] screen [%lu %lu] to client %ls"
                , m_hWnd
                , _point.x
                , _point.y
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        bool result = this->getCursorPosition_( cPos, _position );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::getCursorPosition_( POINT _clientPoint, mt::vec2f * const _position ) const
    {
        float x = static_cast<float>(_clientPoint.x);
        float y = static_cast<float>(_clientPoint.y);

        RECT rect;
        if( ::GetClientRect( m_hWnd, &rect ) == FALSE )
        {
            LOGGER_ERROR( "invalid hwnd [%p] get client rect %ls"
                , m_hWnd
                , Helper::Win32GetLastErrorMessageW()
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
    bool Win32PlatformService::createDirectoryUser_( const WChar * _userPath, const WChar * _directoryPath, const WChar * _file, const void * _data, size_t _size )
    {
        WPath szPath = {L'\0'};
        Helper::pathCorrectForwardslashToW( szPath, _userPath );

        WPath pathCorrect = {L'\0'};
        Helper::pathCorrectForwardslashToW( pathCorrect, _directoryPath );

        WPath fileCorrect = {L'\0'};
        Helper::pathCorrectForwardslashToW( fileCorrect, _file );

        StdString::wcscat( szPath, pathCorrect );

        if( Helper::Win32ExistFile( szPath ) == false )
        {
            if( Helper::Win32CreateDirectory( _userPath, _directoryPath ) == false )
            {
                LOGGER_ERROR( "directory '%ls:%ls' invalid createDirectory '%ls'"
                    , pathCorrect
                    , fileCorrect
                    , szPath
                );

                return false;
            }
        }

        StdString::wcscat( szPath, fileCorrect );

        HANDLE hFile = ::CreateFile( szPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

        if( hFile == INVALID_HANDLE_VALUE )
        {
            LOGGER_ERROR( "'%ls:%ls' invalid createFile '%ls' %ls"
                , pathCorrect
                , fileCorrect
                , szPath
                , Helper::Win32GetLastErrorMessageW()
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
    bool Win32PlatformService::getSpecialFolderPath_( DWORD _flag, WChar * const _path ) const
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

        ::PathAddBackslashW( _path );

        Helper::pathCorrectForwardslashW( _path );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath )
    {
        LOGGER_INFO( "platform", "update desktop wallpaper directory '%s' path '%s'"
            , _directoryPath
            , _filePath
        );

        WPath unicode_directoryPath = {L'\0'};
        if( Helper::utf8ToUnicode( _directoryPath, unicode_directoryPath, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WPath unicode_filePath = {L'\0'};
        if( Helper::utf8ToUnicode( _filePath, unicode_filePath, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WPath szPath = {L'\0'};
        if( this->getSpecialFolderPath_( CSIDL_COMMON_PICTURES | CSIDL_FLAG_CREATE, szPath ) == false )
        {
            LOGGER_ERROR( "invalid getSpecialFolderPath CSIDL_COMMON_PICTURES '%s%s'"
                , _directoryPath
                , _filePath
            );

            return false;
        }

        WPath unicode_correctDirectoryPath = {L'\0'};
        Helper::pathCorrectForwardslashToW( unicode_correctDirectoryPath, unicode_directoryPath );

        StdString::wcscat( szPath, unicode_correctDirectoryPath );

        WPath unicode_correctFilePath = {L'\0'};
        Helper::pathCorrectForwardslashToW( unicode_correctFilePath, unicode_filePath );

        StdString::wcscat( szPath, unicode_correctFilePath );

        if( Helper::Win32ExistFile( szPath ) == false )
        {
            return false;
        }

        if( ::SystemParametersInfo( SPI_SETDESKWALLPAPER, 0, szPath, SPIF_UPDATEINIFILE ) == FALSE )
        {
            LOGGER_ERROR( "SystemParametersInfo [%ls] get error %ls"
                , szPath
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        LOGGER_INFO( "platform", "create directory user picture '%s' path '%s'"
            , _directoryPath
            , _filePath
        );

        WPath unicode_directoryPath = {L'\0'};
        if( Helper::utf8ToUnicode( _directoryPath, unicode_directoryPath, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WPath unicode_filePath = {L'\0'};
        if( Helper::utf8ToUnicode( _filePath, unicode_filePath, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WPath szPath = {L'\0'};
        if( this->getSpecialFolderPath_( CSIDL_COMMON_PICTURES | CSIDL_FLAG_CREATE, szPath ) == false )
        {
            LOGGER_ERROR( "invalid SHGetFolderPath CSIDL_COMMON_PICTURES '%s%s'"
                , _directoryPath
                , _filePath
            );

            return false;
        }

        if( this->createDirectoryUser_( szPath, unicode_directoryPath, unicode_filePath, _data, _size ) == false )
        {
            LOGGER_ERROR( "'%s%s' invalid createDirectoryUser_ '%ls'"
                , _directoryPath
                , _filePath
                , szPath
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        LOGGER_INFO( "platform", "create directory user music '%s' path '%s'"
            , _directoryPath
            , _filePath
        );

        WPath unicode_path = {L'\0'};
        if( Helper::utf8ToUnicode( _directoryPath, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WPath unicode_file = {L'\0'};
        if( Helper::utf8ToUnicode( _filePath, unicode_file, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WPath szPath = {L'\0'};
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
            LOGGER_ERROR( "'%s%s' invalid createDirectoryUser_ '%ls'"
                , _directoryPath
                , _filePath
                , szPath
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::isNeedWindowRender() const
    {
        if( ::IsIconic( m_hWnd ) == TRUE )
        {
            return false;
        }

        if( m_windowExposed == true )
        {
            return true;
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
    DynamicLibraryInterfacePtr Win32PlatformService::loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentInterfacePtr & _doc )
    {
        LOGGER_MESSAGE( "load dynamic library: %s"
            , _dynamicLibraryName
        );

        Win32DynamicLibraryPtr dynamicLibrary = m_factoryDynamicLibraries->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( dynamicLibrary, "invalid create dynamic library" );

        dynamicLibrary->setName( _dynamicLibraryName );

        if( dynamicLibrary->load() == false )
        {
            LOGGER_ERROR( "can't load dynamic library: %s [invalid load]"
                , _dynamicLibraryName
            );

            return nullptr;
        }

        return dynamicLibrary;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::getDesktopResolution( Resolution * const _resolution ) const
    {
        int32_t cxscreen = ::GetSystemMetrics( SM_CXSCREEN );

        if( cxscreen == 0 )
        {
            LOGGER_ERROR( "GetSystemMetrics SM_CXSCREEN invalid %ls"
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        int32_t cyscreen = ::GetSystemMetrics( SM_CYSCREEN );

        if( cyscreen == 0 )
        {
            LOGGER_ERROR( "GetSystemMetrics SM_CYSCREEN invalid %ls"
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        _resolution->setWidth( cxscreen );
        _resolution->setHeight( cyscreen );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::getNoFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::getAlwaysFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32PlatformService::getCurrentPath( Char * const _currentPath ) const
    {
        const Char * option_workdir;
        if( HAS_OPTION_VALUE( "workdir", &option_workdir ) == true )
        {
            StdString::strcpy_safe( _currentPath, option_workdir, MENGINE_MAX_PATH );

            Helper::pathCorrectBackslashA( _currentPath );

            size_t option_workdir_len = StdString::strlen( _currentPath );

            return option_workdir_len;
        }

        WPath currentDirectory = {L'\0'};
        DWORD currentDirectoryLen = ::GetCurrentDirectory( MENGINE_MAX_PATH, currentDirectory );

        if( currentDirectoryLen == 0 )
        {
            LOGGER_ERROR( "GetCurrentDirectory invalid %ls"
                , Helper::Win32GetLastErrorMessageW()
            );

            _currentPath[0] = '\0';

            return 0;
        }

        ::PathRemoveBackslashW( currentDirectory );
        ::PathAddBackslashW( currentDirectory );

        Helper::pathCorrectBackslashW( currentDirectory );

        size_t currentPathLen;
        if( Helper::unicodeToUtf8( currentDirectory, _currentPath, MENGINE_MAX_PATH, &currentPathLen ) == false )
        {
            _currentPath[0] = '\0';

            return 0;
        }

        return currentPathLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32PlatformService::getUserPath( Char * const _userPath ) const
    {
        bool developmentMode = Helper::isDevelopmentMode();
        bool OPTION_roamingMode = HAS_OPTION( "roaming" );
        bool OPTION_noroamingMode = HAS_OPTION( "noroaming" );

        if( (developmentMode == true && OPTION_roamingMode == false) || OPTION_noroamingMode == true )
        {
            WPath currentDirectory = {L'\0'};
            DWORD currentDirectoryLen = ::GetCurrentDirectory( MENGINE_MAX_PATH, currentDirectory );

            if( currentDirectoryLen == 0 )
            {
                LOGGER_ERROR( "failed to get current directory" );

                return 0;
            }

            ::PathRemoveBackslashW( currentDirectory );
            ::PathAddBackslashW( currentDirectory );

            StdString::wcscat( currentDirectory, MENGINE_DEVELOPMENT_USER_FOLDER_NAME );

            uint32_t Engine_BotId = CONFIG_VALUE_INTEGER( "Engine", "BotId", ~0U );

            if( Engine_BotId != ~0U || HAS_OPTION( "bot" ) == true )
            {
                uint32_t botId = GET_OPTION_VALUE_UINT32( "bot", Engine_BotId );

                WChar suffixBuffer[32 + 1] = {L'\0'};
                Helper::stringalized( botId, suffixBuffer, 32 );

                StdString::wcscat( currentDirectory, suffixBuffer );
            }

            ::PathAddBackslashW( currentDirectory );

            Helper::pathCorrectBackslashW( currentDirectory );

            size_t userPathLen;
            if( Helper::unicodeToUtf8( currentDirectory, _userPath, MENGINE_MAX_PATH, &userPathLen ) == false )
            {
                LOGGER_ERROR( "invalid convert path from unicode to utf8 '%ls'"
                    , currentDirectory
                );

                return 0;
            }

            return userPathLen;
        }

        LPITEMIDLIST itemIDList;
        HRESULT hr = ::SHGetSpecialFolderLocation( NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, &itemIDList );

        if( hr != S_OK )
        {
            LOGGER_ERROR( "SHGetSpecialFolderLocation invalid error: %ls [%ld] "
                , Helper::Win32GetErrorMessageW( hr )
                , hr
            );

            return 0;
        }

        WPath currentPath = {L'\0'};
        BOOL result = ::SHGetPathFromIDListW( itemIDList, currentPath );

        if( result == FALSE )
        {
            LOGGER_ERROR( "SHGetPathFromIDListW invalid get error: %ls"
                , Helper::Win32GetLastErrorMessageW()
            );

            return 0;
        }

        ::CoTaskMemFree( itemIDList );

        const Char * Project_Company = CONFIG_VALUE_STRING( "Project", "Company", "UNKNOWN" );

        if( StdString::strlen( Project_Company ) == 0 )
        {
            LOGGER_ERROR( "invalid get company name" );

            return 0;
        }

        WChar companyNameW[MENGINE_APPLICATION_COMPANY_MAXNAME + 1] = {L'\0'};
        if( Helper::utf8ToUnicode( Project_Company, companyNameW, MENGINE_APPLICATION_COMPANY_MAXNAME ) == false )
        {
            LOGGER_ERROR( "invalid convert company name from utf8 to unicode '%s'"
                , Project_Company
            );

            return 0;
        }

        WPath roamingPath = {L'\0'};
        ::PathCombineW( roamingPath, currentPath, companyNameW );

        const Char * Project_Name = CONFIG_VALUE_STRING( "Project", "Name", "UNKNOWN" );

        if( StdString::strlen( Project_Name ) == 0 )
        {
            LOGGER_ERROR( "invalid get project name" );

            return 0;
        }

        WChar projectNameW[MENGINE_APPLICATION_PROJECT_MAXNAME + 1] = {L'\0'};
        if( Helper::utf8ToUnicode( Project_Name, projectNameW, MENGINE_APPLICATION_PROJECT_MAXNAME ) == false )
        {
            LOGGER_ERROR( "invalid convert project name from utf8 to unicode '%s'"
                , Project_Name
            );

            return 0;
        }

        ::PathCombineW( roamingPath, roamingPath, projectNameW );

        ::PathAddBackslashW( roamingPath );

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
    size_t Win32PlatformService::getExtraPreferencesFolderName( Char * const _folderName ) const
    {
        const Char * Project_ExtraPreferencesFolderName = CONFIG_VALUE_STRING( "Project", "ExtraPreferencesFolderName", "" );

        MENGINE_ASSERTION_FATAL( Helper::isCorrectFolderPathA( Project_ExtraPreferencesFolderName ) == true, "invalid extra preferences folder name '%s'"
            , Project_ExtraPreferencesFolderName
        );

        StdString::strcpy_safe( _folderName, Project_ExtraPreferencesFolderName, MENGINE_MAX_PATH );

        size_t Project_ExtraPreferencesFolderNameLen = StdString::strlen( Project_ExtraPreferencesFolderName );

        return Project_ExtraPreferencesFolderNameLen;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::getUserLocaleLanguage( Char * const _userLocaleLanguage ) const
    {
        // Get ISO 639 language code Set1

        WCHAR unicode_userLocaleISO639[LOCALE_NAME_MAX_LENGTH + 1] = {L'\0'};
        if( ::GetUserDefaultLocaleName( unicode_userLocaleISO639, LOCALE_NAME_MAX_LENGTH ) == 0 )
        {
            LOGGER_ERROR( "invalid get user default locale name %ls"
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }
        
        Char utf8_userLocaleISO639[LOCALE_NAME_MAX_LENGTH * 4 + 1] = {'\0'};

        size_t locale_len;
        if( Helper::unicodeToUtf8( unicode_userLocaleISO639, utf8_userLocaleISO639, LOCALE_NAME_MAX_LENGTH * 4, &locale_len ) == false )
        {
            LOGGER_ERROR( "invalid convert locale '%ls' from unicode to utf8"
                , unicode_userLocaleISO639
            );

            return false;
        }

        if( locale_len < MENGINE_LOCALE_LANGUAGE_SIZE )
        {
            LOGGER_ERROR( "invalid locale '%s' len %zu < %d"
                , utf8_userLocaleISO639
                , locale_len
                , MENGINE_LOCALE_LANGUAGE_SIZE
            );

            return false;
        }

        StdString::strcpy_safe( _userLocaleLanguage, utf8_userLocaleISO639, MENGINE_LOCALE_LANGUAGE_SIZE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::closeWindow()
    {
        m_close = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::minimizeWindow()
    {
        LOGGER_INFO( "platform", "minimize window" );

        if( ::ShowWindow( m_hWnd, SW_MINIMIZE ) == FALSE )
        {
            LOGGER_ERROR( "invalid hwnd [%p] ShowWindow %ls"
                , m_hWnd
                , Helper::Win32GetLastErrorMessageW()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::setCursorPosition( const mt::vec2f & _position )
    {
        POINT cPosition;
        cPosition.x = (int32_t)_position.x;
        cPosition.y = (int32_t)_position.y;

        if( ::ClientToScreen( m_hWnd, &cPosition ) == FALSE )
        {
            return;
        }

        ::SetCursorPos( cPosition.x, cPosition.y );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::setCursorIcon( const ConstString & _icon )
    {
        LOGGER_INFO( "platform", "set cursor icon '%s'"
            , _icon.c_str()
        );

        MapCursors::const_iterator it_found = m_cursors.find( _icon );

        MENGINE_ASSERTION_FATAL( it_found != m_cursors.end(), "not found cursor '%s'"
            , _icon.c_str()
        );

        const CursorDesc & desc = it_found->second;

        m_cursor = desc.cursor;

        if( m_cursorMode == true )
        {
            ::SetCursor( m_cursor );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::hasCursorIcon( const ConstString & _icon ) const
    {
        MapCursors::const_iterator it_found = m_cursors.find( _icon );

        if( it_found == m_cursors.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::showKeyboard()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::hideKeyboard()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::isShowKeyboard() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::onEvent( const ConstString & _event, const Params & _params )
    {
        MENGINE_UNUSED( _event );
        MENGINE_UNUSED( _params );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::openUrlInDefaultBrowser( const Char * _url )
    {
        LOGGER_INFO( "platform", "open url in default browser '%s'"
            , _url
        );

        WChar unicode_url[4096 + 1] = {L'\0'};
        if( Helper::utf8ToUnicode( _url, unicode_url, 4096 ) == false )
        {
            return false;
        }

        SHELLEXECUTEINFO ExecuteInfo;
        ExecuteInfo.cbSize = sizeof( ExecuteInfo );
        ExecuteInfo.fMask = 0;
        ExecuteInfo.hwnd = 0;
        ExecuteInfo.lpVerb = L"open";
        ExecuteInfo.lpFile = unicode_url;
        ExecuteInfo.lpParameters = 0;
        ExecuteInfo.lpDirectory = 0;
        ExecuteInfo.nShow = SW_SHOWNORMAL;
        ExecuteInfo.hInstApp = 0;

        if( ::ShellExecuteEx( &ExecuteInfo ) == FALSE )
        {
            LOGGER_ERROR( "ShellExecuteEx [%ls] get error %ls"
                , unicode_url
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::openMail( const Char * _email, const Char * _subject, const Char * _body )
    {
        MENGINE_UNUSED( _email );
        MENGINE_UNUSED( _subject );
        MENGINE_UNUSED( _body );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::openDeleteAccount()
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::setActive_( bool _active )
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
            if( m_cursorMode == true )
            {
                if( m_cursor == NULL )
                {
                    m_cursor = ::LoadCursor( NULL, IDC_ARROW );
                }

                ::SetCursor( m_cursor );
            }
        }
        else
        {
            if( m_cursorMode == true )
            {
                ::SetCursor( NULL );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::messageBox( const Char * _caption, const Char * _format, ... ) const
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        Char str[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};
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
    bool Win32PlatformService::setClipboardText( const Char * _value ) const
    {
        size_t len = StdString::strlen( _value );
        size_t len_alloc = len + 1;

        HGLOBAL hGlb = ::GlobalAlloc( GMEM_MOVEABLE, len_alloc );

        if( hGlb == NULL )
        {
            LOGGER_ERROR( "failed GlobalAlloc [%zu] %ls"
                , len_alloc
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        LPVOID memGlb = ::GlobalLock( hGlb );

        if( memGlb == NULL )
        {
            LOGGER_ERROR( "failed GlobalLock %ls"
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        StdString::memcpy( memGlb, _value, len );

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
    bool Win32PlatformService::getClipboardText( Char * _value, size_t _capacity ) const
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

        StdString::strcpy_safe( _value, clipboardText, _capacity );

        ::GlobalUnlock( hGlb );

        ::CloseClipboard();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId Win32PlatformService::addWin32ProcessHandler( const LambdaWin32ProcessHandler & _lambda, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        UniqueId id = Helper::generateUniqueIdentity();

        Win32ProcessDesc desc;
        desc.id = id;
        desc.lambda = _lambda;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_win32ProcessHandlers.emplace_back( desc );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::removeWin32ProcessHandler( UniqueId _id )
    {
        VectorWin32ProcessHandler::iterator it_found = StdAlgorithm::find_if( m_win32ProcessHandlers.begin(), m_win32ProcessHandlers.end(), [_id]( const Win32ProcessDesc & _desc )
        {
            return _desc.id == _id;
        } );

        MENGINE_ASSERTION_FATAL( it_found != m_win32ProcessHandlers.end(), "not found process handler '%u'"
            , _id
        );

        m_win32ProcessHandlers.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformService::initializeFileService()
    {
        Path currentPath = {'\0'};
        size_t currentPathLen = PLATFORM_SERVICE()
            ->getCurrentPath( currentPath );

        if( currentPathLen == 0 )
        {
            LOGGER_ERROR( "failed to get current directory" );

            return false;
        }

        LOGGER_INFO_PROTECTED( "platform", "current path: %s"
            , currentPath
        );

        FileGroupInterfacePtr defaultFileGroup = nullptr;

        if( FILE_SERVICE()
            ->mountFileGroup( ConstString::none(), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "dir" ), &defaultFileGroup, false, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "failed to mount application directory: '%s'"
                , currentPath
            );

            return false;
        }

        VOCABULARY_SET( FileGroupInterface, STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none(), defaultFileGroup, MENGINE_DOCUMENT_FACTORABLE );

#if !defined(MENGINE_MASTER_RELEASE)
        FileGroupInterfacePtr globalFileGroup = nullptr;

        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "global" ), &globalFileGroup, false, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "failed to mount dev directory: '%s'"
                , currentPath
            );

            return false;
        }

        VOCABULARY_SET( FileGroupInterface, STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ), globalFileGroup, MENGINE_DOCUMENT_FACTORABLE );
#endif

        WPath winDir = {L'\0'};
        UINT winDirLen = ::GetWindowsDirectory( winDir, MENGINE_MAX_PATH );

        if( winDirLen == 0 )
        {
            LOGGER_ERROR( "failed GetWindowsDirectory get error %ls"
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        if( winDirLen >= MENGINE_MAX_PATH )
        {
            LOGGER_ERROR( "failed GetWindowsDirectory len %u great capacity %u"
                , winDirLen
                , MENGINE_MAX_PATH
            );

            return false;
        }

        ::PathRemoveBackslashW( winDir );
        ::PathAddBackslashW( winDir );

        Path utf8_winDir = {'\0'};
        Helper::unicodeToUtf8( winDir, utf8_winDir, MENGINE_MAX_PATH );

        FilePath winDirPath = Helper::stringizeFilePath( utf8_winDir );

        FileGroupInterfacePtr windowsFileGroup;

        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "windows" ), nullptr, nullptr, winDirPath, STRINGIZE_STRING_LOCAL( "global" ), &windowsFileGroup, false, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "failed to mount dev directory: '%s'"
                , currentPath
            );

            return false;
        }

        VOCABULARY_SET( FileGroupInterface, STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "windows" ), windowsFileGroup, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PlatformService::finalizeFileService()
    {
        FILE_SERVICE()
            ->unmountFileGroup( STRINGIZE_STRING_LOCAL( "windows" ) );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        FILE_SERVICE()
            ->unmountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ) );
#endif

        FILE_SERVICE()
            ->unmountFileGroup( ConstString::none() );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "windows" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );
    }
    //////////////////////////////////////////////////////////////////////////
}
