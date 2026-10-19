#include "Win32WebViewApplication.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/LifecycleServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/WebViewBootstrapperInterface.h"
#include "Interface/WebViewPlatformServiceInterface.h"

#include "Runtime/MengineRuntime.h"

#if defined(MENGINE_WEBVIEW_MCP)
#include "Win32WebViewMCP.h"
#endif

#include "Environment/Windows/Win32UnicodeHelper.h"
#include "Environment/Windows/Win32ResolutionHelper.h"
#include "Environment/Windows/Win32WebViewPlatformServiceExtensionInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/StdioLogger.h"
#include "Kernel/Logger.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/Resolution.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( WebViewPlatformService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32WebViewApplication::Win32WebViewApplication()
        : m_window( nullptr )
        , m_instance( nullptr )
        , m_previousTime( 0 )
        , m_running( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32WebViewApplication::~Win32WebViewApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32WebViewApplication::initialize( HINSTANCE _instance, int _argc, WChar ** _argv )
    {
        m_instance = _instance;
        WPath executable = {L'\0'};
        DWORD length = ::GetModuleFileNameW( nullptr, executable, MENGINE_MAX_PATH );

        if( length == 0 )
        {
            return false;
        }

        if( length >= MENGINE_MAX_PATH )
        {
            return false;
        }

        ::PathRemoveFileSpecW( executable );
        WPath content = {L'\0'};

        if( ::PathCombineW( content, executable, L"WebView\\" ) == nullptr )
        {
            return false;
        }

        Configuration configuration;
        configuration.nopause = true;

        for( int index = 1; index != _argc; ++index )
        {
            if( StdString::wcscmp( _argv[index], L"--windowhidden" ) == 0 )
            {
                configuration.hideWindow = true;
            }

            if( StdString::wcscmp( _argv[index], L"--cli" ) == 0 )
            {
                configuration.silentDialog = true;
            }
        }

        if( Helper::Win32UnicodeToUtf8( content, configuration.dataDirectory, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        if( API_MengineCreate( &configuration ) == nullptr )
        {
            return false;
        }

        if( API_MengineBootstrap() == false )
        {
            return false;
        }

        LoggerInterfacePtr logger = Helper::makeFactorableUnique<StdioLogger>( nullptr );
        logger->setWriteHistory( true );

        if( LOGGER_SERVICE()
            ->registerLogger( logger ) == false )
        {
            return false;
        }

        m_logger = logger;

        WPath temporary = {L'\0'};
        DWORD temporaryLength = ::GetTempPathW( MENGINE_MAX_PATH, temporary );

        if( temporaryLength == 0 )
        {
            LOGGER_ERROR( "cannot obtain temporary directory" );

            return false;
        }

        if( temporaryLength >= MENGINE_MAX_PATH )
        {
            LOGGER_ERROR( "cannot obtain temporary directory" );

            return false;
        }

        Path temporaryUTF8 = {'\0'};
        if( Helper::Win32UnicodeToUtf8( temporary, temporaryUTF8, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR( "invalid temporary directory encoding" );

            return false;
        }

        FilePath temporaryPath = Helper::stringizeFilePath( temporaryUTF8 );

        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "temporary" ), nullptr, nullptr, temporaryPath, STRINGIZE_STRING_LOCAL( "global" ), nullptr, true, nullptr ) == false )
        {
            return false;
        }

        const FilePath & rootPath = FilePath::none();

        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "webview" ), nullptr, nullptr, rootPath, STRINGIZE_STRING_LOCAL( "dir" ), nullptr, false, nullptr ) == false )
        {
            return false;
        }

        if( SERVICE_CREATE_SAFE( WebViewPlatformService, nullptr ) == false )
        {
            return false;
        }

        if( API_MengineRun() == false )
        {
            LOGGER_ERROR( "cannot run WebView runtime" );

            return false;
        }

        WNDCLASSW windowClass{};
        windowClass.lpfnWndProc = &Win32WebViewApplication::windowProcedure;
        windowClass.hInstance = _instance;
        windowClass.hCursor = ::LoadCursorW( nullptr, IDC_ARROW );
        windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        windowClass.lpszClassName = L"MengineWebViewWindow";

        if( ::RegisterClassW( &windowClass ) == 0 )
        {
            LOGGER_ERROR( "cannot register WebView window class" );

            return false;
        }

        Resolution windowResolution = CONFIG_VALUE_RESOLUTION( "Window", "Size", Resolution( 1024, 768 ) );
        RECT windowRect;
        Helper::Win32ResolutionToRect( windowResolution, &windowRect );

        if( ::IsRectEmpty( &windowRect ) == TRUE )
        {
            return false;
        }

        if( ::AdjustWindowRectEx( &windowRect, WS_OVERLAPPEDWINDOW, FALSE, 0 ) == FALSE )
        {
            LOGGER_ERROR( "cannot adjust WebView window size" );

            return false;
        }

        int windowWidth = windowRect.right - windowRect.left;
        int windowHeight = windowRect.bottom - windowRect.top;

        int left = configuration.hideWindow == true ? -32000 : CW_USEDEFAULT;
        m_window = ::CreateWindowExW( 0, windowClass.lpszClassName, L"Mengine WebView", WS_OVERLAPPEDWINDOW,
            left, left, windowWidth, windowHeight, nullptr, nullptr, _instance, this );

        if( m_window == nullptr )
        {
            LOGGER_ERROR( "cannot create WebView window" );

            return false;
        }

        PWSTR localData = nullptr;
        HRESULT folderResult = ::SHGetKnownFolderPath( FOLDERID_LocalAppData, KF_FLAG_CREATE, nullptr, &localData );

        if( FAILED( folderResult ) == true )
        {
            LOGGER_ERROR( "cannot obtain LocalAppData directory" );

            return false;
        }

        WPath profilePath = {L'\0'};
        bool combined = ::PathCombineW( profilePath, localData, MENGINE_WEBVIEW_PROFILE_NAME ) != nullptr;
        ::CoTaskMemFree( localData );

        if( combined == false )
        {
            LOGGER_ERROR( "cannot combine WebView profile directory" );

            return false;
        }

        Helper::pathCorrectForwardslashW( profilePath );

        int created = ::SHCreateDirectoryExW( nullptr, profilePath, nullptr );

        if( created != ERROR_SUCCESS && created != ERROR_ALREADY_EXISTS && created != ERROR_FILE_EXISTS )
        {
            LOGGER_ERROR( "cannot create WebView profile directory" );

            return false;
        }

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "webview" ) );
        MemoryInterfacePtr bridgeMemory = Helper::createMemoryFile( fileGroup, STRINGIZE_FILEPATH_LOCAL( "MengineWebViewBridge.js" ), false, false, nullptr );

        if( bridgeMemory == nullptr )
        {
            LOGGER_ERROR( "cannot read MengineWebViewBridge.js" );

            return false;
        }

        Win32WebViewPlatformServiceExtensionInterface * platform = WEBVIEW_PLATFORM_SERVICE()
            ->getUnknown();

        if( platform->attachWindow( m_window, profilePath, bridgeMemory ) == false )
        {
            LOGGER_ERROR( "cannot attach WebView window" );

            return false;
        }

        if( platform->openContent( fileGroup, STRINGIZE_FILEPATH_LOCAL( "index.html" ) ) == false )
        {
            LOGGER_ERROR( "cannot open WebView content index.html" );

            return false;
        }
#if defined(MENGINE_WEBVIEW_MCP)
        m_mcp = Helper::makeFactorableUnique<Win32WebViewMCP>( nullptr );

        if( m_mcp->initialize( m_window, _argc, _argv ) == false )
        {
            LOGGER_ERROR( "cannot initialize WebView MCP adapter" );

            return false;
        }
#endif
        m_previousTime = ::GetTickCount64();

        if( ::SetTimer( m_window, 1, 10, nullptr ) == 0 )
        {
            LOGGER_ERROR( "cannot start WebView runtime timer" );

            return false;
        }

        m_running = true;
        ::ShowWindow( m_window, SW_SHOWNOACTIVATE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    int Win32WebViewApplication::loop()
    {
        for( ;; )
        {
            MSG message{};
            BOOL result = ::GetMessageW( &message, nullptr, 0, 0 );

            if( result == -1 )
            {
                return 1;
            }

            if( result == 0 )
            {
                return static_cast<int>(message.wParam);
            }

            ::TranslateMessage( &message );
            ::DispatchMessageW( &message );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    LRESULT CALLBACK Win32WebViewApplication::windowProcedure( HWND _window, UINT _message, WPARAM _wparam, LPARAM _lparam )
    {
        LONG_PTR applicationValue = ::GetWindowLongPtrW( _window, GWLP_USERDATA );
        Win32WebViewApplication * application = reinterpret_cast<Win32WebViewApplication *>(applicationValue);

        if( _message == WM_NCCREATE )
        {
            CREATESTRUCTW * creation = reinterpret_cast<CREATESTRUCTW *>(_lparam);
            application = static_cast<Win32WebViewApplication *>(creation->lpCreateParams);
            LONG_PTR windowData = reinterpret_cast<LONG_PTR>(application);
            ::SetWindowLongPtrW( _window, GWLP_USERDATA, windowData );
        }

        if( application != nullptr )
        {
            LRESULT result = application->handleMessage( _window, _message, _wparam, _lparam );

            return result;
        }

        LRESULT result = ::DefWindowProcW( _window, _message, _wparam, _lparam );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    LRESULT Win32WebViewApplication::handleMessage( HWND _window, UINT _message, WPARAM _wparam, LPARAM _lparam )
    {
        if( m_running == true )
        {
            Win32WebViewPlatformServiceExtensionInterface * platform = WEBVIEW_PLATFORM_SERVICE()
                ->getUnknown();

            switch( _message )
            {
            case WM_SIZE:
                {
                    platform->resizeBrowser();
                }break;
            case WM_COMMAND:
                {
                    if( HIWORD( _wparam ) == BN_CLICKED )
                    {
                        platform->retryBrowser();
                    }
                }break;
            case WM_TIMER:
                {
                    ULONGLONG now = ::GetTickCount64();
                    float milliseconds = static_cast<float>(now - m_previousTime);
                    m_previousTime = now;
                    WEBVIEW_BOOTSTRAPPER_SERVICE()
                        ->tick( milliseconds );
#if defined(MENGINE_WEBVIEW_MCP)
                    m_mcp->tick();
#endif
                }break;
            case WM_ACTIVATEAPP:
                {
                    if( _wparam != 0 )
                    {
                        LIFECYCLE_SERVICE()
                            ->onApplicationWillEnterForeground();
                        LIFECYCLE_SERVICE()
                            ->onApplicationDidBecomeActive();
                    }
                    else
                    {
                        LIFECYCLE_SERVICE()
                            ->onApplicationWillResignActive();
                        LIFECYCLE_SERVICE()
                            ->onApplicationDidEnterBackground();
                    }
                }break;
            case WM_CLOSE:
                {
                    platform->close();
                    ::DestroyWindow( _window );

                    return 0;
                }
            default:
                {
                }break;
            }
        }

        if( _message == WM_DESTROY )
        {
            m_window = nullptr;
            m_running = false;
            ::PostQuitMessage( 0 );

            return 0;
        }

        LRESULT result = ::DefWindowProcW( _window, _message, _wparam, _lparam );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewApplication::finalize()
    {
        m_running = false;

        if( SERVICE_PROVIDER_EXIST() == true )
        {
            if( SERVICE_IS_INITIALIZE( LifecycleServiceInterface ) == true )
            {
                LIFECYCLE_SERVICE()
                    ->onApplicationWillTerminate();
            }

            SERVICE_FINALIZE( WebViewPlatformService );
            SERVICE_DESTROY( WebViewPlatformService );

#if defined(MENGINE_WEBVIEW_MCP)
            if( m_mcp != nullptr )
            {
                m_mcp->finalize();
                m_mcp = nullptr;
            }
#endif

            if( SERVICE_IS_INITIALIZE( WebViewBootstrapperInterface ) == true )
            {
                WEBVIEW_BOOTSTRAPPER_SERVICE()
                    ->stop();
            }

            if( SERVICE_IS_INITIALIZE( FileServiceInterface ) == true )
            {
                if( FILE_SERVICE()
                    ->hasFileGroup( STRINGIZE_STRING_LOCAL( "webview" ), nullptr ) == true )
                {
                    FILE_SERVICE()
                        ->unmountFileGroup( STRINGIZE_STRING_LOCAL( "webview" ) );
                }

                if( FILE_SERVICE()
                    ->hasFileGroup( STRINGIZE_STRING_LOCAL( "temporary" ), nullptr ) == true )
                {
                    FILE_SERVICE()
                        ->unmountFileGroup( STRINGIZE_STRING_LOCAL( "temporary" ) );
                }
            }

            if( m_logger != nullptr )
            {
                LOGGER_SERVICE()
                    ->unregisterLogger( m_logger );
                m_logger = nullptr;
            }

            API_MengineFinalize();
        }

        if( m_window != nullptr )
        {
            ::DestroyWindow( m_window );
            m_window = nullptr;
        }

        if( m_instance != nullptr )
        {
            ::UnregisterClassW( L"MengineWebViewWindow", m_instance );
            m_instance = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
