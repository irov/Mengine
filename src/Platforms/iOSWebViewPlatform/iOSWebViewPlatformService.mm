#include "iOSWebViewPlatformService.h"

#include "Interface/WebViewBridgeServiceInterface.h"

#import "Platforms/AppleWebViewPlatform/AppleWebViewController.h"

#import "Environment/Apple/AppleString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( WebViewPlatformService, Mengine::iOSWebViewPlatformService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSWebViewPlatformService::iOSWebViewPlatformService()
        : m_window( nil )
        , m_controller( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSWebViewPlatformService::~iOSWebViewPlatformService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSWebViewPlatformService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSWebViewPlatformService::_finalizeService()
    {
        m_externalNavigation = nullptr;
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSWebViewPlatformService::attachScene( UIWindowScene * _scene )
    {
        if( _scene == nil )
        {
            return false;
        }

        if( m_window != nil )
        {
            return false;
        }

        m_controller = [[AppleWebViewController alloc] init];

        if( [m_controller initializeBrowser] == NO )
        {
            this->close();

            return false;
        }

        [m_controller setExternalNavigation:m_externalNavigation];

        m_window = [[UIWindow alloc] initWithWindowScene:_scene];
        m_window.rootViewController = m_controller;
        [m_window makeKeyAndVisible];

        WebViewBridgeSenderInterfacePtr sender = WebViewBridgeSenderInterfacePtr::from( this );

        WEBVIEW_BRIDGE_SERVICE()
            ->setSender( sender );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    UIWindow * iOSWebViewPlatformService::getUIWindow() const
    {
        return m_window;
    }
    //////////////////////////////////////////////////////////////////////////
    WKWebView * iOSWebViewPlatformService::getWKWebView() const
    {
        WKWebView * browser = m_controller.m_webView;

        return browser;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSWebViewPlatformService::openContent( const FileGroupInterfacePtr & _fileGroup, const FilePath & _entry )
    {
        if( [m_controller openContent:_fileGroup entry:_entry] == NO )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSWebViewPlatformService::openURL( const String & _url )
    {
        NSString * url = [AppleString NSStringFromString:_url];

        if( [m_controller openURL:url] == NO )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSWebViewPlatformService::reload()
    {
        [m_controller reload];
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSWebViewPlatformService::goBack()
    {
        if( [m_controller goBack] == NO )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSWebViewPlatformService::goForward()
    {
        if( [m_controller goForward] == NO )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSWebViewPlatformService::sendJSON( const jpp::object & _message )
    {
        if( [m_controller sendJSON:_message] == NO )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object iOSWebViewPlatformService::getCapabilities() const
    {
        jpp::object capabilities = jpp::make_object();
        capabilities.set( "snapshot", true );
        capabilities.set( "persistentStorage", true );
        capabilities.set( "localOrigin", "mengine://localhost" );

        return capabilities;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSWebViewPlatformService::takeSnapshot( const LambdaSnapshot & _callback )
    {
        if( m_controller == nil )
        {
            Data data;
            _callback( data, 0, 0 );

            return;
        }

        [m_controller takeSnapshot:_callback];
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSWebViewPlatformService::setExternalNavigationCallback( const LambdaExternalNavigation & _callback )
    {
        m_externalNavigation = _callback;
        [m_controller setExternalNavigation:_callback];
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSWebViewPlatformService::close()
    {
        [m_controller close];
        m_controller = nil;

        m_window.hidden = YES;
        m_window.rootViewController = nil;
        m_window = nil;

        WEBVIEW_BRIDGE_SERVICE()
            ->setSender( nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSWebViewPlatformService::onWebViewBridgeSenderSendJSON( const jpp::object & _message )
    {
        if( this->sendJSON( _message ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
