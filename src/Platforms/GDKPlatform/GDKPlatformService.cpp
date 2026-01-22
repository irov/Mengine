#include "GDKPlatformService.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/ProfilerSystemInterface.h"
#include "Interface/DateTimeSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/SoundServiceInterface.h"
#include "Interface/PluginInterface.h"
#include "Interface/EnvironmentServiceInterface.h"

#include "Environment/Windows/WindowsIncluder.h"
#include "Environment/Windows/Win32Helper.h"
#include "Environment/Windows/Win32FileHelper.h"

#include "Environment/GDK/GDKIncluder.h"

#include "GDKDynamicLibrary.h"

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

#ifndef MENGINE_DEVELOPMENT_USER_FOLDER_NAME
#define MENGINE_DEVELOPMENT_USER_FOLDER_NAME L"User"
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_WINDOW_CLASSNAME
#define MENGINE_WINDOW_CLASSNAME "GDKWindow"
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PlatformService, Mengine::GDKPlatformService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static LRESULT CALLBACK wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
        {
            switch( uMsg )
            {
            case WM_CREATE:
                {
                    LPCREATESTRUCTW createStruct = (LPCREATESTRUCTW)lParam;

                    GDKPlatformService * platform = (GDKPlatformService *)createStruct->lpCreateParams;

#if defined(MENGINE_PLATFORM_WINDOWS64)
                    ::SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)platform );
#else
                    ::SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG)platform );
#endif

                    return (LRESULT)NULL;
                }
                break;
            }

            GDKPlatformService * platform = nullptr;

#if defined(MENGINE_PLATFORM_WINDOWS64)
            platform = (GDKPlatformService *)::GetWindowLongPtr( hWnd, GWLP_USERDATA );
#else
            platform = (GDKPlatformService *)::GetWindowLongPtr( hWnd, GWLP_USERDATA );
#endif

            if( platform == nullptr )
            {
                LRESULT result = ::DefWindowProc( hWnd, uMsg, wParam, lParam );

                return result;
            }

            // For now, just use default window proc
            // Can be extended later if needed
            LRESULT result = ::DefWindowProc( hWnd, uMsg, wParam, lParam );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    GDKPlatformService::GDKPlatformService()
        : m_beginTime( 0 )
        , m_hWnd( NULL )
        , m_fullscreen( false )
        , m_active( false )
        , m_close( false )
        , m_touchpad( false )
        , m_desktop( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GDKPlatformService::~GDKPlatformService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::_initializeService()
    {
        // Initialize GDK Game Runtime as early as possible
        HRESULT hr = ::XGameRuntimeInitialize();
        if( FAILED( hr ) )
        {
            LOGGER_ERROR( "failed XGameRuntimeInitialize [0x%08lX]"
                , hr
            );

            return false;
        }

        LOGGER_INFO( "platform", "GDK Game Runtime initialized successfully" );

        m_beginTime = Helper::getSystemTimestamp();

        const Char * option_platforms[MENGINE_OPTIONS_VALUES_MAX];
        uint32_t option_platforms_count;
        if( OPTIONS_SERVICE()
            ->getOptionValues( "platform", option_platforms, &option_platforms_count ) == true )
        {
            for( uint32_t index = 0; index != option_platforms_count; ++index )
            {
                const Char * option_platform = option_platforms[index];

                m_platformTags.addTag( Helper::stringizeString( option_platform ) );
            }
        }
        else
        {
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "GDK" ) );

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

        LOGGER_INFO( "platform", "GDK Platform initialized" );

        PathString Window_ClassName = CONFIG_VALUE_PATHSTRING( "Window", "ClassName", MENGINE_WINDOW_CLASSNAME );

        Helper::utf8ToUnicode( Window_ClassName, m_windowClassName.data(), MENGINE_MAX_PATH );

        m_factoryDynamicLibraries = Helper::makeFactoryPool<GDKDynamicLibrary, 8>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::_finalizeService()
    {
        m_active = false;

        m_platformTags.clear();

        // Ensure all GDK handles are closed before uninitializing
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDynamicLibraries );

        m_factoryDynamicLibraries = nullptr;

        // Uninitialize GDK Game Runtime as late as possible in shutdown sequence
        ::XGameRuntimeUninitialize();

        LOGGER_INFO( "platform", "GDK Game Runtime uninitialized" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::_runService()
    {
        PlatformDateTime dateTime;
        DATETIME_SYSTEM()
            ->getLocalDateTime( &dateTime );

        LOGGER_INFO( "platform", "GDK platform start date: %02u.%02u.%u, %02u:%02u:%02u"
            , dateTime.day
            , dateTime.month
            , dateTime.year
            , dateTime.hour
            , dateTime.minute
            , dateTime.second
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::_stopService()
    {
        PlatformDateTime dateTime;
        DATETIME_SYSTEM()
            ->getLocalDateTime( &dateTime );

        LOGGER_INFO( "platform", "GDK platform stop date: %02u.%02u.%u, %02u:%02u:%02u"
            , dateTime.day
            , dateTime.month
            , dateTime.year
            , dateTime.hour
            , dateTime.minute
            , dateTime.second
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::initializeFileService()
    {
        Path currentPath = {'\0'};
        size_t currentPathLen = this->getCurrentPath( currentPath );

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
            LOGGER_ERROR( "failed to mount windows directory: '%s'"
                , currentPath
            );

            return false;
        }

        VOCABULARY_SET( FileGroupInterface, STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "windows" ), windowsFileGroup, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::finalizeFileService()
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

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );
#endif

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp GDKPlatformService::getPlatfomTime() const
    {
        Timestamp currentTime = Helper::getSystemTimestamp();

        Timestamp platformTime = currentTime - m_beginTime;

        return platformTime;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::runPlatform()
    {
        if( m_close == true )
        {
            return false;
        }

        m_active = true;

        if( this->updatePlatform() == false )
        {
            return false;
        }

        this->tickPlatform( 0.f );

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_RUN );

        LOGGER_INFO( "platform", "GDK run platform" );

        MENGINE_PROFILER_BEGIN_APPLICATION();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::loopPlatform()
    {
        double prevTime = Helper::getElapsedTime();

        while( m_close == false )
        {
            MENGINE_PROFILER_FRAME( "main" );

            NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_UPDATE );

            if( this->updatePlatform() == false )
            {
                break;
            }

            double currentTime = Helper::getElapsedTime();

            float frameTime = (float)(currentTime - prevTime);

            prevTime = currentTime;

            if( m_active == false )
            {
                ::Sleep( 100 );

                continue;
            }

            this->tickPlatform( frameTime );

            if( this->renderPlatform() == false )
            {
                ::Sleep( 100 );

                continue;
            }

            if( APPLICATION_SERVICE()
                ->getVSync() == false )
            {
                ::Sleep( 1 );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::updatePlatform()
    {
        if( m_close == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::tickPlatform( float _frameTime )
    {
        bool updating = APPLICATION_SERVICE()
            ->beginUpdate();

        if( updating == true )
        {
            APPLICATION_SERVICE()
                ->tick( _frameTime );
        }

        APPLICATION_SERVICE()
            ->endUpdate();
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::renderPlatform()
    {
        bool successful = APPLICATION_SERVICE()
            ->render();

        if( successful == false )
        {
            return false;
        }

        APPLICATION_SERVICE()
            ->flush();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::stopPlatform()
    {
        LOGGER_INFO( "platform", "GDK stop platform" );

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_STOP );

        MENGINE_PROFILER_END_APPLICATION();
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::setProjectTitle( const Char * _projectTitle )
    {
        if( _projectTitle == nullptr )
        {
            m_projectTitle.clear();

            return;
        }

        Helper::utf8ToUnicodeSize( _projectTitle, MENGINE_UNKNOWN_SIZE, m_projectTitle.data(), MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME );

        LOGGER_INFO( "platform", "GDK project title: %s"
            , _projectTitle
        );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t GDKPlatformService::getProjectTitle( Char * const _projectTitle ) const
    {
        size_t utf8Size;
        Helper::unicodeToUtf8( m_projectTitle.c_str(), _projectTitle, MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME, &utf8Size );

        return utf8Size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::alreadyRunningMonitor()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::createWindow( const Resolution & _resolution, bool _fullscreen )
    {
        m_windowResolution = _resolution;
        m_fullscreen = _fullscreen;

        bool Platform_WithoutWindow = CONFIG_VALUE_BOOLEAN( "Platform", "WithoutWindow", false );

        if( Platform_WithoutWindow == true || HAS_OPTION( "norender" ) == true )
        {
            LOGGER_INFO( "platform", "platform without window" );

            return true;
        }

        HINSTANCE hInstance = ::GetModuleHandle( NULL );

        WNDCLASSEX wc;
        ::ZeroMemory( &wc, sizeof( WNDCLASSEX ) );
        wc.cbSize = sizeof( WNDCLASSEX );
        wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = &Detail::wndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = NULL;
        wc.hCursor = ::LoadCursor( NULL, IDC_ARROW );
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
            , NULL
            , NULL
            , hInstance
            , (LPVOID)this );

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
    bool GDKPlatformService::atachWindow( HWND _hWnd, bool _fullscreen )
    {
        MENGINE_ASSERTION_FATAL( _hWnd != NULL, "atachWindow hWnd is NULL" );

        m_hWnd = _hWnd;
        m_fullscreen = _fullscreen;

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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::detachWindow()
    {
        m_hWnd = NULL;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & GDKPlatformService::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::hasPlatformTag( const ConstString & _tag ) const
    {
        bool result = m_platformTags.hasTag( _tag );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::isDesktop() const
    {
        return m_desktop;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::hasTouchpad() const
    {
        return m_touchpad;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t GDKPlatformService::dpToWidthPx( int32_t _dp ) const
    {
        return _dp;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t GDKPlatformService::dpToHeightPx( int32_t _dp ) const
    {
        return _dp;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t GDKPlatformService::pxToWidthDp( int32_t _px ) const
    {
        return _px;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t GDKPlatformService::pxToHeightDp( int32_t _px ) const
    {
        return _px;
    }
    //////////////////////////////////////////////////////////////////////////
    DynamicLibraryInterfacePtr GDKPlatformService::loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentInterfacePtr & _doc )
    {
        GDKDynamicLibraryPtr dynamicLibrary = m_factoryDynamicLibraries->createObject( _doc );

        dynamicLibrary->setName( _dynamicLibraryName );

        if( dynamicLibrary->load() == false )
        {
            LOGGER_ERROR( "invalid load dynamic library '%s'"
                , _dynamicLibraryName
            );

            return nullptr;
        }

        return dynamicLibrary;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::getDesktopResolution( Resolution * const _resolution ) const
    {
        _resolution->setWidth( 1920 );
        _resolution->setHeight( 1080 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::getNoFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::getAlwaysFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t GDKPlatformService::getCurrentPath( Char * const _currentPath ) const
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

        if( currentDirectoryLen >= MENGINE_MAX_PATH )
        {
            LOGGER_ERROR( "GetCurrentDirectory len %lu great capacity %u"
                , currentDirectoryLen
                , MENGINE_MAX_PATH
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
    size_t GDKPlatformService::getUserPath( Char * const _userPath ) const
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

            StdString::wcscat_safe( currentDirectory, MENGINE_DEVELOPMENT_USER_FOLDER_NAME, MENGINE_MAX_PATH );

            uint32_t Engine_BotId = CONFIG_VALUE_INTEGER( "Engine", "BotId", ~0U );

            if( Engine_BotId != ~0U || HAS_OPTION( "bot" ) == true )
            {
                uint32_t botId = GET_OPTION_VALUE_UINT32( "bot", Engine_BotId );

                WChar suffixBuffer[32 + 1] = {L'\0'};
                Helper::stringalized( botId, suffixBuffer, 32 );

                StdString::wcscat_safe( currentDirectory, suffixBuffer, MENGINE_MAX_PATH );
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

            ::CoTaskMemFree( itemIDList );

            return 0;
        }

        ::CoTaskMemFree( itemIDList );

        PathString Project_Company = CONFIG_VALUE_PATHSTRING( "Project", "Company", "UNKNOWN" );

        if( Project_Company.empty() == true )
        {
            LOGGER_ERROR( "invalid get company name" );

            return 0;
        }

        WChar companyNameW[MENGINE_APPLICATION_COMPANY_MAXNAME + 1] = {L'\0'};
        if( Helper::utf8ToUnicode( Project_Company, companyNameW, MENGINE_APPLICATION_COMPANY_MAXNAME ) == false )
        {
            LOGGER_ERROR( "invalid convert company name from utf8 to unicode '%s'"
                , Project_Company.c_str()
            );

            return 0;
        }

        WPath roamingPath = {L'\0'};
        ::PathCombineW( roamingPath, currentPath, companyNameW );

        PathString Project_Name = CONFIG_VALUE_PATHSTRING( "Project", "Name", "UNKNOWN" );

        if( Project_Name.empty() == true )
        {
            LOGGER_ERROR( "invalid get project name" );

            return 0;
        }

        WChar projectNameW[MENGINE_APPLICATION_PROJECT_MAXNAME + 1] = {L'\0'};
        if( Helper::utf8ToUnicode( Project_Name, projectNameW, MENGINE_APPLICATION_PROJECT_MAXNAME ) == false )
        {
            LOGGER_ERROR( "invalid convert project name from utf8 to unicode '%s'"
                , Project_Name.c_str()
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
    size_t GDKPlatformService::getExtraPreferencesFolderName( Char * const _folderName ) const
    {
        _folderName[0] = '\0';

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::getUserLocaleLanguage( Char * const _userLocaleLanguage ) const
    {
        LANGID langId = ::GetUserDefaultUILanguage();

        Char lang[8] = {'\0'};
        if( ::GetLocaleInfoA( langId, LOCALE_SISO639LANGNAME, lang, 8 ) == 0 )
        {
            return false;
        }

        StdString::strcpy( _userLocaleLanguage, lang );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::closeWindow()
    {
        m_close = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::minimizeWindow()
    {
        if( m_hWnd != NULL )
        {
            ::ShowWindow( m_hWnd, SW_MINIMIZE );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::setCursorPosition( const mt::vec2f & _cursorPosition )
    {
        MENGINE_UNUSED( _cursorPosition );

        //GDK implementation
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::setCursorIcon( const ConstString & _cursorIcon )
    {
        MENGINE_UNUSED( _cursorIcon );

        //GDK implementation
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::hasCursorIcon( const ConstString & _cursorIcon ) const
    {
        MENGINE_UNUSED( _cursorIcon );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::showKeyboard()
    {
        //GDK implementation
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::hideKeyboard()
    {
        //GDK implementation
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::isShowKeyboard() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
        m_windowResolution = _resolution;
        m_fullscreen = _fullscreen;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::notifyVsyncChanged( bool _vsync )
    {
        MENGINE_UNUSED( _vsync );

        //GDK implementation
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::notifyCursorModeChanged( bool _cursorMode )
    {
        MENGINE_UNUSED( _cursorMode );

        //GDK implementation
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::notifyCursorIconSetup( const ConstString & _name, const ContentInterfacePtr & _content, const MemoryInterfacePtr & _buffer )
    {
        MENGINE_UNUSED( _name );
        MENGINE_UNUSED( _content );
        MENGINE_UNUSED( _buffer );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::onEvent( const ConstString & _event, const Params & _params )
    {
        MENGINE_UNUSED( _event );
        MENGINE_UNUSED( _params );

        //GDK implementation
    }
    //////////////////////////////////////////////////////////////////////////
    size_t GDKPlatformService::getShortPathName( const Char * _path, Char * const _short ) const
    {
        WPath unicode_path = {L'\0'};
        if( Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return 0;
        }

        WPath unicode_short = {L'\0'};
        DWORD len = ::GetShortPathName( unicode_path, unicode_short, MENGINE_MAX_PATH );

        if( Helper::unicodeToUtf8Size( unicode_short, (size_t)len, _short, MENGINE_MAX_PATH ) == false )
        {
            return 0;
        }

        return (size_t)len;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t GDKPlatformService::getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const
    {
        MENGINE_UNUSED( _fontName );

        _fontPath[0] = '\0';
        *_groupName = STRINGIZE_STRING_LOCAL( "windows" );

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::getMaxClientResolution( Resolution * const _resolution ) const
    {
        _resolution->setWidth( 3840 );
        _resolution->setHeight( 2160 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::openUrlInDefaultBrowser( const Char * _url )
    {
        WPath unicode_url = {L'\0'};
        if( Helper::utf8ToUnicode( _url, unicode_url, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        HINSTANCE result = ::ShellExecute( NULL, L"open", unicode_url, NULL, NULL, SW_SHOWNORMAL );

        if( (INT_PTR)result <= 32 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::openMail( const Char * _email, const Char * _subject, const Char * _body )
    {
        MENGINE_UNUSED( _email );
        MENGINE_UNUSED( _subject );
        MENGINE_UNUSED( _body );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::openDeleteAccount()
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _data );
        MENGINE_UNUSED( _size );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _data );
        MENGINE_UNUSED( _size );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::messageBox( const Char * _caption, const Char * _format, ... ) const
    {
        Char str[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        int32_t size_vsnprintf = MENGINE_VSNPRINTF( str, MENGINE_LOGGER_MAX_MESSAGE - 1, _format, args );

        MENGINE_VA_LIST_END( args );

        if( size_vsnprintf < 0 )
        {
            LOGGER_ERROR( "invalid message box format message '%s'"
                , _format
            );

            return;
        }

        WPath unicode_caption = {L'\0'};
        Helper::utf8ToUnicode( _caption, unicode_caption, MENGINE_MAX_PATH );

        WPath unicode_message = {L'\0'};
        Helper::utf8ToUnicode( str, unicode_message, MENGINE_MAX_PATH );

        ::MessageBox( NULL, unicode_message, unicode_caption, MB_OK | MB_ICONINFORMATION );
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::setClipboardText( const Char * _value ) const
    {
        if( ::OpenClipboard( NULL ) == FALSE )
        {
            return false;
        }

        ::EmptyClipboard();

        WPath unicode_value = {L'\0'};
        if( Helper::utf8ToUnicode( _value, unicode_value, MENGINE_MAX_PATH ) == false )
        {
            ::CloseClipboard();

            return false;
        }

        size_t unicode_size = StdString::wcslen( unicode_value );
        HGLOBAL hMem = ::GlobalAlloc( GMEM_MOVEABLE, (unicode_size + 1) * sizeof( WChar ) );

        if( hMem == NULL )
        {
            ::CloseClipboard();

            return false;
        }

        WChar * pMem = (WChar *)::GlobalLock( hMem );
        StdString::wcscpy( pMem, unicode_value );
        ::GlobalUnlock( hMem );

        ::SetClipboardData( CF_UNICODETEXT, hMem );
        ::CloseClipboard();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::getClipboardText( Char * _value, size_t _capacity ) const
    {
        if( ::OpenClipboard( NULL ) == FALSE )
        {
            return false;
        }

        HGLOBAL hMem = ::GetClipboardData( CF_UNICODETEXT );

        if( hMem == NULL )
        {
            ::CloseClipboard();

            return false;
        }

        WChar * pMem = (WChar *)::GlobalLock( hMem );

        size_t utf8_size;
        if( Helper::unicodeToUtf8( pMem, _value, _capacity, &utf8_size ) == false )
        {
            ::GlobalUnlock( hMem );
            ::CloseClipboard();

            return false;
        }

        ::GlobalUnlock( hMem );
        ::CloseClipboard();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::isDebuggerPresent() const
    {
        if( ::IsDebuggerPresent() == FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::debugBreak()
    {
        ::DebugBreak();
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::setHWNDIcon( const WChar * _iconResource )
    {
        MENGINE_UNUSED( _iconResource );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    DWORD GDKPlatformService::getWindowStyle_( bool _fullscreen ) const
    {
        MENGINE_UNUSED( _fullscreen );

        DWORD dwStyle = WS_OVERLAPPED;

        dwStyle |= WS_POPUP;
        dwStyle |= WS_VISIBLE;

        if( _fullscreen == false )
        {
            dwStyle |= WS_CAPTION;

            dwStyle |= WS_SYSMENU;
            dwStyle |= WS_MINIMIZEBOX;
        }

        return dwStyle;
    }
    //////////////////////////////////////////////////////////////////////////
    DWORD GDKPlatformService::getWindowExStyle_( bool _fullscreen ) const
    {
        MENGINE_UNUSED( _fullscreen );

        DWORD dwExStyle = 0;

        dwExStyle |= WS_EX_APPWINDOW;

        if( _fullscreen == false )
        {
            dwExStyle |= WS_EX_WINDOWEDGE;
        }

        return dwExStyle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKPlatformService::calcWindowsRect_( const Resolution & _resolution, bool _fullscreen, RECT * const _rect ) const
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

        if( _fullscreen == false )
        {
            uint32_t OPTION_winx = GET_OPTION_VALUE_UINT32( "winx", MENGINE_UNKNOWN_COUNT );
            uint32_t OPTION_winy = GET_OPTION_VALUE_UINT32( "winy", MENGINE_UNKNOWN_COUNT );

            if( OPTION_winx != MENGINE_UNKNOWN_COUNT && OPTION_winy != MENGINE_UNKNOWN_COUNT )
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
                DWORD dwStyle = this->getWindowStyle_( _fullscreen );
                DWORD dwStyleEx = this->getWindowExStyle_( _fullscreen );

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
    HWND GDKPlatformService::getWindowHandle() const
    {
        return m_hWnd;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId GDKPlatformService::addWin32ProcessHandler( const LambdaWin32ProcessHandler & _lambda, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _lambda );
        MENGINE_UNUSED( _doc );

        return INVALID_UNIQUE_ID;
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKPlatformService::removeWin32ProcessHandler( UniqueId _id )
    {
        MENGINE_UNUSED( _id );

        //GDK implementation
    }
    //////////////////////////////////////////////////////////////////////////
}
