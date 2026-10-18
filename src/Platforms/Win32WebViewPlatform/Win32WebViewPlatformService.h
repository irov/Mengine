#pragma once

#include "Interface/WebViewPlatformServiceInterface.h"
#include "Interface/WebViewBridgeSenderInterface.h"

#include "Win32WebViewIncluder.h"

#include "Environment/Windows/Win32WebViewPlatformServiceExtensionInterface.h"
#include "Environment/Windows/Win32RuntimeClassWeak.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Map.h"
#include "Kernel/WString.h"

#include "Config/Path.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32WebViewPlatformService
        : public ServiceBase<WebViewPlatformServiceInterface>
        , public WebViewBridgeSenderInterface
        , public Win32WebViewPlatformServiceExtensionInterface
    {
        DECLARE_UNKNOWABLE();

    public:
        Win32WebViewPlatformService();
        ~Win32WebViewPlatformService() override;

    public:
        void setExternalNavigationCallback( const LambdaExternalNavigation & _callback ) override;
        jpp::object getCapabilities() const override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool attachWindow( HWND _window, const WChar * _profile, const MemoryInterfacePtr & _bridge ) override;
        void resizeBrowser() override;
        void retryBrowser() override;

    public:
        bool openContent( const FileGroupInterfacePtr & _fileGroup, const FilePath & _entry ) override;
        bool openURL( const String & _url ) override;
        void reload() override;
        bool goBack() override;
        bool goForward() override;
        bool sendJSON( const jpp::object & _message ) override;
        void takeSnapshot( const LambdaSnapshot & _callback ) override;
        void close() override;

    protected:
        bool onWebViewBridgeSenderSendJSON( const jpp::object & _message ) override;

    protected:
        typedef Win32RuntimeClassWeakPtr<Win32WebViewPlatformService> WeakPlatformServicePtr;
        typedef Microsoft::WRL::ComPtr<ICoreWebView2Environment> ICoreWebView2EnvironmentPtr;
        typedef Microsoft::WRL::ComPtr<ICoreWebView2Controller> ICoreWebView2ControllerPtr;
        typedef Microsoft::WRL::ComPtr<ICoreWebView2> ICoreWebView2Ptr;
        typedef Microsoft::WRL::ComPtr<ICoreWebView2_3> ICoreWebView2_3Ptr;
        typedef Microsoft::WRL::ComPtr<IStream> IStreamPtr;

        typedef Map<uint64_t, LambdaSnapshot> MapSnapshots;

    protected:
        void createBrowser_();
        void closeBrowser_();
        HRESULT configureBrowser_();
        void failBrowser_( HRESULT _error, const WChar * _reason = L"WebView2 initialization failed" );
        void cancelDocument_();
        void connectDocument_();
        void showOverlay_( const WChar * _text, bool _retry );
        bool navigate_( const WString & _url );

    protected:
        void onEnvironmentCreated_( HRESULT _result, ICoreWebView2Environment * _environment );
        void onControllerCreated_( HRESULT _result, ICoreWebView2Controller * _controller );
        void onNewWindowRequested_( ICoreWebView2NewWindowRequestedEventArgs * _arguments );
        void onDocumentScriptAdded_( HRESULT _result );
        void onNavigationStarting_( ICoreWebView2NavigationStartingEventArgs * _arguments );
        void onContentLoading_( ICoreWebView2ContentLoadingEventArgs * _arguments );
        void onWebMessageReceived_( ICoreWebView2WebMessageReceivedEventArgs * _arguments );
        void onNavigationCompleted_( ICoreWebView2NavigationCompletedEventArgs * _arguments );

    protected:
        HWND m_window;
        HWND m_overlay;
        HWND m_retry;

        WPath m_profile;
        WString m_bridge;
        WPath m_folder;
        WString m_url;
        WString m_allowedURL;

        bool m_browserReady;
        bool m_localDocument;
        bool m_documentCommitted;
        uint64_t m_navigation;
        uint64_t m_snapshotSequence;

        LambdaExternalNavigation m_externalNavigation;
        MapSnapshots m_snapshots;

        WeakPlatformServicePtr m_weak;
        ICoreWebView2EnvironmentPtr m_environment;
        ICoreWebView2ControllerPtr m_controller;
        ICoreWebView2Ptr m_browser;
    };
    //////////////////////////////////////////////////////////////////////////
}
