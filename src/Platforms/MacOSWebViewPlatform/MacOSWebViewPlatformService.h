#pragma once

#include "Interface/WebViewPlatformServiceInterface.h"
#include "Interface/WebViewBridgeSenderInterface.h"

#include "Environment/MacOS/MacOSWebViewPlatformServiceExtensionInterface.h"

#include "Kernel/ServiceBase.h"

@class AppleWebViewController;

namespace Mengine
{
    class MacOSWebViewPlatformService
        : public ServiceBase<WebViewPlatformServiceInterface>
        , public WebViewBridgeSenderInterface
        , public MacOSWebViewPlatformServiceExtensionInterface
    {
        DECLARE_UNKNOWABLE();

    public:
        MacOSWebViewPlatformService();
        ~MacOSWebViewPlatformService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool createWindow( NSString * _title, bool _hidden ) override;
        NSWindow * getNSWindow() const override;
        WKWebView * getWKWebView() const override;

    public:
        bool openContent( const FileGroupInterfacePtr & _fileGroup, const FilePath & _entry ) override;
        bool openURL( const String & _url ) override;
        void reload() override;
        bool goBack() override;
        bool goForward() override;
        bool sendJSON( const jpp::object & _message ) override;
        jpp::object getCapabilities() const override;
        void takeSnapshot( const LambdaSnapshot & _callback ) override;
        void setExternalNavigationCallback( const LambdaExternalNavigation & _callback ) override;
        void close() override;

    protected:
        bool onWebViewBridgeSenderSendJSON( const jpp::object & _message ) override;

    protected:
        NSWindow * m_window;
        AppleWebViewController * m_controller;
        LambdaExternalNavigation m_externalNavigation;
    };
}
