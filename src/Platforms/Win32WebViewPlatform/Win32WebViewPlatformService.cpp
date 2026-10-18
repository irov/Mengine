#include "Win32WebViewPlatformService.h"

#include "Interface/WebViewBridgeServiceInterface.h"

#include "Environment/Windows/Win32UnicodeHelper.h"

#include "Kernel/JSONHelper.h"
#include "Kernel/StringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/UnicodeHelper.h"

#include "Config/StdInt.h"
#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( WebViewPlatformService, Mengine::Win32WebViewPlatformService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const size_t MAX_MESSAGE_SIZE = 1024 * 1024;
        static const size_t MAX_SNAPSHOTS = 128;
        static const uint64_t MAX_SNAPSHOT_SIZE = 8 * 1024 * 1024;
        static const int OVERLAY_HORIZONTAL_MARGIN = 16;
        static const int OVERLAY_VERTICAL_DIVISOR = 3;
        static const int OVERLAY_HEIGHT = 100;
        static const int RETRY_WIDTH = 120;
        static const int RETRY_HEIGHT = 36;
        static const int RETRY_VERTICAL_GAP = 10;
        //////////////////////////////////////////////////////////////////////////
        static bool readSnapshot( IStream * _stream, Data * const _data )
        {
            STATSTG info{};
            HRESULT statResult = _stream->Stat( &info, STATFLAG_NONAME );

            if( FAILED( statResult ) == true )
            {
                return false;
            }

            if( info.cbSize.QuadPart > MAX_SNAPSHOT_SIZE )
            {
                return false;
            }

            LARGE_INTEGER start{};
            HRESULT seekResult = _stream->Seek( start, STREAM_SEEK_SET, nullptr );

            if( FAILED( seekResult ) == true )
            {
                return false;
            }

            ULONG size = static_cast<ULONG>(info.cbSize.QuadPart);
            _data->resize( size );

            void * buffer = _data->data();
            ULONG read = 0;
            HRESULT result = _stream->Read( buffer, size, &read );

            if( FAILED( result ) == true )
            {
                _data->clear();

                return false;
            }

            if( read != size )
            {
                _data->clear();

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool isHTTPS( const WChar * _url, bool _local )
        {
            URL_COMPONENTSW components{};
            components.dwStructSize = sizeof( components );
            components.dwHostNameLength = static_cast<DWORD>(-1);
            components.dwUserNameLength = static_cast<DWORD>(-1);
            components.dwPasswordLength = static_cast<DWORD>(-1);

            if( ::InternetCrackUrlW( _url, 0, 0, &components ) == FALSE )
            {
                return false;
            }

            if( components.nScheme != INTERNET_SCHEME_HTTPS )
            {
                return false;
            }

            if( components.dwUserNameLength != 0 )
            {
                return false;
            }

            if( components.dwPasswordLength != 0 )
            {
                return false;
            }

            if( _local == false )
            {
                return true;
            }

            if( components.nPort != INTERNET_DEFAULT_HTTPS_PORT )
            {
                return false;
            }

            int length = static_cast<int>(components.dwHostNameLength);
            int comparison = ::CompareStringOrdinal( components.lpszHostName, length, L"mengine.localhost", -1, TRUE );

            if( comparison != CSTR_EQUAL )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Win32WebViewPlatformService::Win32WebViewPlatformService()
        : m_window( nullptr )
        , m_overlay( nullptr )
        , m_retry( nullptr )
        , m_profile{L'\0'}
        , m_folder{L'\0'}
        , m_browserReady( false )
        , m_localDocument( false )
        , m_documentCommitted( false )
        , m_navigation( 0 )
        , m_snapshotSequence( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32WebViewPlatformService::~Win32WebViewPlatformService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32WebViewPlatformService::_initializeService()
    {
        WebViewBridgeSenderInterfacePtr sender = WebViewBridgeSenderInterfacePtr::from( this );

        WEBVIEW_BRIDGE_SERVICE()
            ->setSender( sender );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::_finalizeService()
    {
        WEBVIEW_BRIDGE_SERVICE()
            ->setSender( nullptr );

        this->close();

        m_externalNavigation = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32WebViewPlatformService::attachWindow( HWND _window, const WChar * _profile, const MemoryInterfacePtr & _bridge )
    {
        if( m_window != nullptr )
        {
            return false;
        }

        if( _window == nullptr )
        {
            return false;
        }

        const Char * bridge = _bridge->getBuffer();
        size_t bridgeSize = _bridge->getSize();

        if( Helper::Win32Utf8ToUnicode( bridge, bridgeSize, &m_bridge ) == false )
        {
            return false;
        }

        m_bridge = L"if (window.top === window && location.origin === 'https://mengine.localhost') {" + m_bridge + L"}";

        StdString::wcscpy_safe( m_profile, _profile, MENGINE_MAX_PATH );

        m_window = _window;

        LONG_PTR instanceValue = ::GetWindowLongPtrW( _window, GWLP_HINSTANCE );
        HINSTANCE instance = reinterpret_cast<HINSTANCE>(instanceValue);
        m_overlay = ::CreateWindowExW( 0, L"STATIC", L"Loading...", WS_CHILD | WS_VISIBLE | SS_CENTER, 0, 0, 0, 0, _window, nullptr, instance, nullptr );

        UINT_PTR retryControlId = 1;
        HMENU retryMenu = reinterpret_cast<HMENU>(retryControlId);
        m_retry = ::CreateWindowExW( 0, L"BUTTON", L"Retry", WS_CHILD | BS_PUSHBUTTON, 0, 0, 0, 0, _window, retryMenu, instance, nullptr );

        if( m_overlay == nullptr )
        {
            this->close();

            return false;
        }

        if( m_retry == nullptr )
        {
            this->close();

            return false;
        }

        this->resizeBrowser();
        this->createBrowser_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::createBrowser_()
    {
        this->showOverlay_( L"Loading...", false );

        m_weak = Helper::makeWin32RuntimeClassWeak( this );

        if( m_weak == nullptr )
        {
            this->failBrowser_( E_OUTOFMEMORY );

            return;
        }

        WeakPlatformServicePtr weak = m_weak;
        HRESULT result = ::CreateCoreWebView2EnvironmentWithOptions( nullptr, m_profile, nullptr, Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [weak]( HRESULT _result, ICoreWebView2Environment * _environment ) -> HRESULT
            {
                Win32WebViewPlatformService * platform = weak->get();

                if( platform == nullptr )
                {
                    return S_OK;
                }

                platform->onEnvironmentCreated_( _result, _environment );

                return S_OK;
            } ).Get() );

        if( FAILED( result ) == true )
        {
            this->failBrowser_( result );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::onEnvironmentCreated_( HRESULT _result, ICoreWebView2Environment * _environment )
    {
        if( FAILED( _result ) == true )
        {
            this->failBrowser_( _result );

            return;
        }

        if( _environment == nullptr )
        {
            this->failBrowser_( _result );

            return;
        }

        m_environment = _environment;

        WeakPlatformServicePtr weak = m_weak;
        HRESULT result = _environment->CreateCoreWebView2Controller( m_window, Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
            [weak]( HRESULT _status, ICoreWebView2Controller * _controller ) -> HRESULT
            {
                Win32WebViewPlatformService * platform = weak->get();

                if( platform == nullptr )
                {
                    if( _controller != nullptr )
                    {
                        _controller->Close();
                    }

                    return S_OK;
                }

                platform->onControllerCreated_( _status, _controller );

                return S_OK;
            } ).Get() );

        if( FAILED( result ) == true )
        {
            this->failBrowser_( result );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::onControllerCreated_( HRESULT _result, ICoreWebView2Controller * _controller )
    {
        if( FAILED( _result ) == true )
        {
            this->failBrowser_( _result );

            return;
        }

        if( _controller == nullptr )
        {
            this->failBrowser_( _result );

            return;
        }

        m_controller = _controller;

        HRESULT browserResult = _controller->get_CoreWebView2( &m_browser );

        if( FAILED( browserResult ) == true )
        {
            this->failBrowser_( browserResult );

            return;
        }

        HRESULT configureResult = this->configureBrowser_();

        if( FAILED( configureResult ) == true )
        {
            this->failBrowser_( configureResult );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    HRESULT Win32WebViewPlatformService::configureBrowser_()
    {
        this->resizeBrowser();

        WeakPlatformServicePtr weak = m_weak;
        EventRegistrationToken token{};
        HRESULT navigationResult = m_browser->add_NavigationStarting( Microsoft::WRL::Callback<ICoreWebView2NavigationStartingEventHandler>(
            [weak]( ICoreWebView2 * _sender, ICoreWebView2NavigationStartingEventArgs * _arguments ) -> HRESULT
            {
                MENGINE_UNUSED( _sender );

                Win32WebViewPlatformService * platform = weak->get();

                if( platform == nullptr )
                {
                    return S_OK;
                }

                platform->onNavigationStarting_( _arguments );

                return S_OK;
            } ).Get(), &token );

        if( FAILED( navigationResult ) == true )
        {
            return navigationResult;
        }

        HRESULT contentLoadingResult = m_browser->add_ContentLoading( Microsoft::WRL::Callback<ICoreWebView2ContentLoadingEventHandler>(
            [weak]( ICoreWebView2 * _sender, ICoreWebView2ContentLoadingEventArgs * _arguments ) -> HRESULT
            {
                MENGINE_UNUSED( _sender );

                Win32WebViewPlatformService * platform = weak->get();

                if( platform == nullptr )
                {
                    return S_OK;
                }

                platform->onContentLoading_( _arguments );

                return S_OK;
            } ).Get(), &token );

        if( FAILED( contentLoadingResult ) == true )
        {
            return contentLoadingResult;
        }

        HRESULT messageResult = m_browser->add_WebMessageReceived( Microsoft::WRL::Callback<ICoreWebView2WebMessageReceivedEventHandler>(
            [weak]( ICoreWebView2 * _sender, ICoreWebView2WebMessageReceivedEventArgs * _arguments ) -> HRESULT
            {
                MENGINE_UNUSED( _sender );

                Win32WebViewPlatformService * platform = weak->get();

                if( platform == nullptr )
                {
                    return S_OK;
                }

                platform->onWebMessageReceived_( _arguments );

                return S_OK;
            } ).Get(), &token );

        if( FAILED( messageResult ) == true )
        {
            return messageResult;
        }

        HRESULT navigationCompletedResult = m_browser->add_NavigationCompleted( Microsoft::WRL::Callback<ICoreWebView2NavigationCompletedEventHandler>(
            [weak]( ICoreWebView2 * _sender, ICoreWebView2NavigationCompletedEventArgs * _arguments ) -> HRESULT
            {
                MENGINE_UNUSED( _sender );

                Win32WebViewPlatformService * platform = weak->get();

                if( platform == nullptr )
                {
                    return S_OK;
                }

                platform->onNavigationCompleted_( _arguments );

                return S_OK;
            } ).Get(), &token );

        if( FAILED( navigationCompletedResult ) == true )
        {
            return navigationCompletedResult;
        }

        HRESULT processFailedResult = m_browser->add_ProcessFailed( Microsoft::WRL::Callback<ICoreWebView2ProcessFailedEventHandler>(
            [weak]( ICoreWebView2 * _sender, ICoreWebView2ProcessFailedEventArgs * _arguments ) -> HRESULT
            {
                MENGINE_UNUSED( _sender );
                MENGINE_UNUSED( _arguments );

                Win32WebViewPlatformService * platform = weak->get();

                if( platform == nullptr )
                {
                    return S_OK;
                }

                platform->failBrowser_( E_FAIL, L"WebView2 browser process failed" );

                return S_OK;
            } ).Get(), &token );

        if( FAILED( processFailedResult ) == true )
        {
            return processFailedResult;
        }

        HRESULT newWindowResult = m_browser->add_NewWindowRequested( Microsoft::WRL::Callback<ICoreWebView2NewWindowRequestedEventHandler>(
            [weak]( ICoreWebView2 * _sender, ICoreWebView2NewWindowRequestedEventArgs * _arguments ) -> HRESULT
            {
                MENGINE_UNUSED( _sender );

                _arguments->put_Handled( TRUE );

                Win32WebViewPlatformService * platform = weak->get();

                if( platform == nullptr )
                {
                    return S_OK;
                }

                platform->onNewWindowRequested_( _arguments );

                return S_OK;
            } ).Get(), &token );

        if( FAILED( newWindowResult ) == true )
        {
            return newWindowResult;
        }

        const WChar * bridge = m_bridge.c_str();
        HRESULT scriptResult = m_browser->AddScriptToExecuteOnDocumentCreated( bridge, Microsoft::WRL::Callback<ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler>(
            [weak]( HRESULT _status, LPCWSTR _id ) -> HRESULT
            {
                MENGINE_UNUSED( _id );

                Win32WebViewPlatformService * platform = weak->get();

                if( platform == nullptr )
                {
                    return S_OK;
                }

                platform->onDocumentScriptAdded_( _status );

                return S_OK;
            } ).Get() );

        return scriptResult;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::onNewWindowRequested_( ICoreWebView2NewWindowRequestedEventArgs * _arguments )
    {
        if( m_externalNavigation == nullptr )
        {
            return;
        }

        LPWSTR uri = nullptr;
        HRESULT result = _arguments->get_Uri( &uri );

        if( FAILED( result ) == true )
        {
            return;
        }

        if( uri == nullptr )
        {
            return;
        }

        String url;
        bool converted = Helper::Win32UnicodeToUtf8( uri, &url );
        ::CoTaskMemFree( uri );

        if( converted == false )
        {
            return;
        }

        LambdaExternalNavigation callback = m_externalNavigation;
        callback( url );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::onDocumentScriptAdded_( HRESULT _result )
    {
        if( FAILED( _result ) == true )
        {
            this->failBrowser_( _result );

            return;
        }

        m_browserReady = true;

        if( m_url.empty() == true )
        {
            return;
        }

        WString url = m_url;

        if( this->navigate_( url ) == false )
        {
            this->failBrowser_( E_FAIL );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::resizeBrowser()
    {
        if( m_window == nullptr )
        {
            return;
        }

        RECT bounds{};
        ::GetClientRect( m_window, &bounds );

        if( m_controller != nullptr )
        {
            m_controller->put_Bounds( bounds );
        }

        int overlayX = Detail::OVERLAY_HORIZONTAL_MARGIN;
        int overlayY = bounds.bottom / Detail::OVERLAY_VERTICAL_DIVISOR;
        int overlayWidth = bounds.right - Detail::OVERLAY_HORIZONTAL_MARGIN * 2;
        ::MoveWindow( m_overlay, overlayX, overlayY, overlayWidth, Detail::OVERLAY_HEIGHT, TRUE );

        int retryX = bounds.right / 2 - Detail::RETRY_WIDTH / 2;
        int retryY = overlayY + Detail::OVERLAY_HEIGHT + Detail::RETRY_VERTICAL_GAP;
        ::MoveWindow( m_retry, retryX, retryY, Detail::RETRY_WIDTH, Detail::RETRY_HEIGHT, TRUE );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::showOverlay_( const WChar * _text, bool _retry )
    {
        ::SetWindowTextW( m_overlay, _text );
        ::ShowWindow( m_overlay, SW_SHOW );

        if( _retry == true )
        {
            ::ShowWindow( m_retry, SW_SHOW );
        }
        else
        {
            ::ShowWindow( m_retry, SW_HIDE );
        }

        if( m_controller == nullptr )
        {
            return;
        }

        m_controller->put_IsVisible( FALSE );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::failBrowser_( HRESULT _error, const WChar * _reason )
    {
        this->closeBrowser_();

        WChar reason[256];
        UINT errorCode = static_cast<UINT>(_error);

        if( _error == HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ) )
        {
            ::wsprintfW( reason, L"WebView2 Runtime was not found (0x%08X). Install Microsoft Edge WebView2 Runtime, then retry.", errorCode );
        }
        else
        {
            ::wsprintfW( reason, L"%s (0x%08X). Retry to reopen the application.", _reason, errorCode );
        }

        LOGGER_ERROR( "WebView2 failure: %ls"
            , reason
        );

        this->showOverlay_( reason, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::cancelDocument_()
    {
        m_localDocument = false;
        m_documentCommitted = false;

        WEBVIEW_BRIDGE_SERVICE()
            ->closeDocument();

        MapSnapshots snapshots;
        snapshots.swap( m_snapshots );

        for( const auto & [id, callback] : snapshots )
        {
            MENGINE_UNUSED( id );

            Data empty;
            callback( empty, 0, 0 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::onNavigationStarting_( ICoreWebView2NavigationStartingEventArgs * _arguments )
    {
        LPWSTR uri = nullptr;
        HRESULT result = _arguments->get_Uri( &uri );

        if( FAILED( result ) == true )
        {
            _arguments->put_Cancel( TRUE );

            return;
        }

        if( uri == nullptr )
        {
            _arguments->put_Cancel( TRUE );

            return;
        }

        WString url = uri;
        ::CoTaskMemFree( uri );

        const WChar * address = url.c_str();
        bool local = Detail::isHTTPS( address, true );

        if( local == false )
        {
            if( url != m_allowedURL )
            {
                _arguments->put_Cancel( TRUE );

                if( m_externalNavigation == nullptr )
                {
                    return;
                }

                String external;
                if( Helper::Win32UnicodeToUtf8( address, &external ) == false )
                {
                    return;
                }

                LambdaExternalNavigation callback = m_externalNavigation;
                callback( external );

                return;
            }
        }

        m_allowedURL.clear();

        _arguments->get_NavigationId( &m_navigation );

        this->cancelDocument_();

        m_localDocument = local;

        WEBVIEW_BRIDGE_SERVICE()
            ->beginDocument();

        this->showOverlay_( L"Loading...", false );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::onContentLoading_( ICoreWebView2ContentLoadingEventArgs * _arguments )
    {
        UINT64 navigation = 0;
        HRESULT navigationResult = _arguments->get_NavigationId( &navigation );

        if( FAILED( navigationResult ) == true )
        {
            return;
        }

        if( navigation != m_navigation )
        {
            return;
        }

        BOOL errorPage = TRUE;
        HRESULT errorPageResult = _arguments->get_IsErrorPage( &errorPage );

        if( FAILED( errorPageResult ) == true )
        {
            return;
        }

        if( errorPage == TRUE )
        {
            this->cancelDocument_();

            return;
        }

        m_documentCommitted = true;
        this->connectDocument_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::connectDocument_()
    {
        if( m_documentCommitted == false )
        {
            return;
        }

        if( m_localDocument == false )
        {
            return;
        }

        uint64_t generation = WEBVIEW_BRIDGE_SERVICE()
            ->getDocumentGeneration();
        String number = Helper::stringUInt64( generation );
        String script = "window.Mengine && window.Mengine.connect('" + number + "')";

        WString unicode;
        if( Helper::Win32Utf8ToUnicode( script, &unicode ) == false )
        {
            return;
        }

        const WChar * code = unicode.c_str();
        m_browser->ExecuteScript( code, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::onWebMessageReceived_( ICoreWebView2WebMessageReceivedEventArgs * _arguments )
    {
        if( m_documentCommitted == false )
        {
            return;
        }

        if( m_localDocument == false )
        {
            return;
        }

        LPWSTR source = nullptr;
        HRESULT result = _arguments->get_Source( &source );

        if( FAILED( result ) == true )
        {
            return;
        }

        if( source == nullptr )
        {
            return;
        }

        bool trusted = Detail::isHTTPS( source, true );
        ::CoTaskMemFree( source );

        if( trusted == false )
        {
            return;
        }

        LPWSTR json = nullptr;
        HRESULT messageResult = _arguments->TryGetWebMessageAsString( &json );

        if( FAILED( messageResult ) == true )
        {
            return;
        }

        if( json == nullptr )
        {
            return;
        }

        String text;
        bool converted = Helper::Win32UnicodeToUtf8( json, &text );
        ::CoTaskMemFree( json );

        if( converted == false )
        {
            return;
        }

        if( text.size() > Detail::MAX_MESSAGE_SIZE )
        {
            return;
        }

        jpp::object message = Helper::loadJSONString( text, nullptr );

        if( message.is_type_object() == false )
        {
            return;
        }

        jpp::object kind;
        if( message.exist( "kind", &kind ) == false )
        {
            return;
        }

        if( kind.is_type_string() == false )
        {
            return;
        }

        String kindValue = kind;

        if( kindValue == "connect" )
        {
            this->connectDocument_();

            return;
        }

        uint64_t generation = WEBVIEW_BRIDGE_SERVICE()
            ->getDocumentGeneration();
        String expected = Helper::stringUInt64( generation );

        jpp::object field;
        if( message.exist( "generation", &field ) == false )
        {
            return;
        }

        if( field.is_type_string() == false )
        {
            return;
        }

        String received = field;

        if( received != expected )
        {
            return;
        }

        WEBVIEW_BRIDGE_SERVICE()
            ->receiveMessage( generation, message );

        bool ready = WEBVIEW_BRIDGE_SERVICE()
            ->isAppReady();

        if( ready == false )
        {
            return;
        }

        ::ShowWindow( m_overlay, SW_HIDE );
        ::ShowWindow( m_retry, SW_HIDE );
        m_controller->put_IsVisible( TRUE );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::onNavigationCompleted_( ICoreWebView2NavigationCompletedEventArgs * _arguments )
    {
        UINT64 navigation = 0;
        _arguments->get_NavigationId( &navigation );

        if( navigation != m_navigation )
        {
            return;
        }

        BOOL success = FALSE;
        _arguments->get_IsSuccess( &success );

        if( success == FALSE )
        {
            LOGGER_ERROR( "WebView2 navigation failed" );

            this->cancelDocument_();
            this->showOverlay_( L"Cannot load the page. Retry to reopen the application.", true );

            return;
        }

        if( m_localDocument == true )
        {
            m_documentCommitted = true;
            this->connectDocument_();

            return;
        }

        ::ShowWindow( m_overlay, SW_HIDE );
        m_controller->put_IsVisible( TRUE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32WebViewPlatformService::openContent( const FileGroupInterfacePtr & _fileGroup, const FilePath & _entry )
    {
        if( _fileGroup == nullptr )
        {
            return false;
        }

        if( _entry.empty() == true )
        {
            return false;
        }

        if( _fileGroup->existFile( _entry, false ) == false )
        {
            return false;
        }

        const FilePath & rootPath = FilePath::none();
        Path root = {'\0'};
        _fileGroup->getFullPath( rootPath, root );

        if( Helper::utf8ToUnicode( root, m_folder, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        String address = "https://mengine.localhost/";
        const Char hex[] = "0123456789ABCDEF";

        for( const Char * cursor = _entry.c_str(); *cursor != '\0'; ++cursor )
        {
            uint8_t character = static_cast<uint8_t>(*cursor);
            bool safe = (character >= 'a' && character <= 'z')
                || (character >= 'A' && character <= 'Z')
                || (character >= '0' && character <= '9')
                || character == '/'
                || character == '-'
                || character == '_'
                || character == '.'
                || character == '~';

            if( safe == true )
            {
                address += static_cast<Char>(character);

                continue;
            }

            uint32_t highNibble = character >> 4;
            uint32_t lowNibble = character & 0x0F;
            address += '%';
            address += hex[highNibble];
            address += hex[lowNibble];
        }

        WString url;
        if( Helper::Win32Utf8ToUnicode( address, &url ) == false )
        {
            return false;
        }

        if( this->navigate_( url ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32WebViewPlatformService::navigate_( const WString & _url )
    {
        m_url = _url;

        if( m_browserReady == false )
        {
            if( m_window == nullptr )
            {
                return false;
            }

            return true;
        }

        if( MENGINE_WPATH_EMPTY( m_folder ) == false )
        {
            ICoreWebView2_3Ptr mapping;
            HRESULT queried = m_browser.As( &mapping );

            if( FAILED( queried ) == true )
            {
                return false;
            }

            HRESULT mapped = mapping->SetVirtualHostNameToFolderMapping( L"mengine.localhost", m_folder, COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_DENY );

            if( FAILED( mapped ) == true )
            {
                return false;
            }
        }

        m_allowedURL = _url;
        const WChar * address = _url.c_str();
        HRESULT result = m_browser->Navigate( address );

        if( FAILED( result ) == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32WebViewPlatformService::openURL( const String & _url )
    {
        WString url;
        if( Helper::Win32Utf8ToUnicode( _url, &url ) == false )
        {
            return false;
        }

        const WChar * address = url.c_str();

        if( Detail::isHTTPS( address, false ) == false )
        {
            return false;
        }

        if( this->navigate_( url ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::reload()
    {
        if( m_browser == nullptr )
        {
            return;
        }

        LPWSTR source = nullptr;
        HRESULT result = m_browser->get_Source( &source );

        if( FAILED( result ) == true )
        {
            m_browser->Reload();

            return;
        }

        if( source == nullptr )
        {
            m_browser->Reload();

            return;
        }

        m_allowedURL = source;
        ::CoTaskMemFree( source );

        m_browser->Reload();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32WebViewPlatformService::goBack()
    {
        if( m_browser == nullptr )
        {
            return false;
        }

        BOOL available = FALSE;
        m_browser->get_CanGoBack( &available );

        if( available == FALSE )
        {
            return false;
        }

        HRESULT result = m_browser->GoBack();

        if( FAILED( result ) == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32WebViewPlatformService::goForward()
    {
        if( m_browser == nullptr )
        {
            return false;
        }

        BOOL available = FALSE;
        m_browser->get_CanGoForward( &available );

        if( available == FALSE )
        {
            return false;
        }

        HRESULT result = m_browser->GoForward();

        if( FAILED( result ) == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32WebViewPlatformService::sendJSON( const jpp::object & _message )
    {
        if( m_browser == nullptr )
        {
            return false;
        }

        if( m_localDocument == false )
        {
            return false;
        }

        if( _message.is_type_object() == false )
        {
            return false;
        }

        if( WEBVIEW_BRIDGE_SERVICE()
            ->isDocumentReady() == false )
        {
            return false;
        }

        jpp::object message = jpp::copy( _message );
        uint64_t generation = WEBVIEW_BRIDGE_SERVICE()
            ->getDocumentGeneration();
        String number = Helper::stringUInt64( generation );
        message.set( "generation", number );

        String json;
        if( Helper::writeJSONStringCompact( message, &json ) == false )
        {
            return false;
        }

        WString text;
        if( Helper::Win32Utf8ToUnicode( json, &text ) == false )
        {
            return false;
        }

        const WChar * data = text.c_str();
        HRESULT result = m_browser->PostWebMessageAsJson( data );

        if( FAILED( result ) == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object Win32WebViewPlatformService::getCapabilities() const
    {
        jpp::object capabilities = jpp::make_object();
        capabilities.set( "snapshot", true );
        capabilities.set( "persistentStorage", true );
        capabilities.set( "localOrigin", "https://mengine.localhost" );

        return capabilities;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::takeSnapshot( const LambdaSnapshot & _callback )
    {
        if( m_browser == nullptr )
        {
            Data empty;
            _callback( empty, 0, 0 );

            return;
        }

        if( m_snapshots.size() >= Detail::MAX_SNAPSHOTS )
        {
            Data empty;
            _callback( empty, 0, 0 );

            return;
        }

        IStreamPtr stream;
        HRESULT created = ::CreateStreamOnHGlobal( nullptr, TRUE, &stream );

        if( FAILED( created ) == true )
        {
            Data empty;
            _callback( empty, 0, 0 );

            return;
        }

        uint64_t id = ++m_snapshotSequence;
        m_snapshots.emplace( id, _callback );

        WeakPlatformServicePtr weak = m_weak;

        RECT bounds{};
        ::GetClientRect( m_window, &bounds );

        HRESULT result = m_browser->CapturePreview( COREWEBVIEW2_CAPTURE_PREVIEW_IMAGE_FORMAT_PNG, stream.Get(), Microsoft::WRL::Callback<ICoreWebView2CapturePreviewCompletedHandler>(
            [weak, stream, id, bounds]( HRESULT _result ) -> HRESULT
            {
                Win32WebViewPlatformService * platform = weak->get();

                if( platform == nullptr )
                {
                    return S_OK;
                }

                MapSnapshots::iterator it = platform->m_snapshots.find( id );

                if( it == platform->m_snapshots.end() )
                {
                    return S_OK;
                }

                LambdaSnapshot callback = it->second;
                platform->m_snapshots.erase( it );

                Data data;

                if( SUCCEEDED( _result ) == true )
                {
                    Detail::readSnapshot( stream.Get(), &data );
                }

                uint32_t width = static_cast<uint32_t>(bounds.right);
                uint32_t height = static_cast<uint32_t>(bounds.bottom);
                callback( data, width, height );

                return S_OK;
            } ).Get() );

        if( FAILED( result ) == true )
        {
            m_snapshots.erase( id );

            Data empty;
            _callback( empty, 0, 0 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::setExternalNavigationCallback( const LambdaExternalNavigation & _callback )
    {
        m_externalNavigation = _callback;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::closeBrowser_()
    {
        m_browserReady = false;

        if( m_weak != nullptr )
        {
            m_weak->reset();
        }

        this->cancelDocument_();

        if( m_controller != nullptr )
        {
            m_controller->Close();
        }

        m_browser.Reset();
        m_controller.Reset();
        m_environment.Reset();
        m_weak.Reset();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::retryBrowser()
    {
        if( m_window == nullptr )
        {
            return;
        }

        this->closeBrowser_();
        this->createBrowser_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewPlatformService::close()
    {
        this->closeBrowser_();

        if( m_overlay != nullptr )
        {
            ::DestroyWindow( m_overlay );

            m_overlay = nullptr;
        }

        if( m_retry != nullptr )
        {
            ::DestroyWindow( m_retry );

            m_retry = nullptr;
        }

        m_window = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32WebViewPlatformService::onWebViewBridgeSenderSendJSON( const jpp::object & _message )
    {
        if( this->sendJSON( _message ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
