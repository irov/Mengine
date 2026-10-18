#pragma once

#include "Interface/WebViewPlatformServiceInterface.h"
#include "Interface/WebViewBridgeSenderInterface.h"

#include "Environment/iOS/iOSWebViewPlatformServiceExtensionInterface.h"

#include "Kernel/ServiceBase.h"

@class AppleWebViewController;

namespace Mengine
{
    class iOSWebViewPlatformService
        : public ServiceBase<WebViewPlatformServiceInterface>
        , public WebViewBridgeSenderInterface
        , public iOSWebViewPlatformServiceExtensionInterface
    {
        DECLARE_UNKNOWABLE();

    public:
        iOSWebViewPlatformService();
        ~iOSWebViewPlatformService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool attachScene( UIWindowScene * _scene ) override;
        UIWindow * getUIWindow() const override;
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
        UIWindow * m_window;
        AppleWebViewController * m_controller;
        LambdaExternalNavigation m_externalNavigation;
    };
}
