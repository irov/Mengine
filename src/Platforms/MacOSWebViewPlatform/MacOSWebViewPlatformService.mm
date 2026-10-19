#include "MacOSWebViewPlatformService.h"

#include "Interface/WebViewBridgeServiceInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Resolution.h"

#import "Platforms/AppleWebViewPlatform/AppleWebViewController.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( WebViewPlatformService, Mengine::MacOSWebViewPlatformService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MacOSWebViewPlatformService::MacOSWebViewPlatformService()
        : m_window( nil )
        , m_controller( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MacOSWebViewPlatformService::~MacOSWebViewPlatformService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSWebViewPlatformService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSWebViewPlatformService::_finalizeService()
    {
        m_externalNavigation = nullptr;
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSWebViewPlatformService::createWindow( NSString * _title, bool _hidden )
    {
        if( m_window != nil )
        {
            return false;
        }

        Resolution windowResolution = CONFIG_VALUE_RESOLUTION( "Window", "Size", Resolution( 1024, 768 ) );
        NSSize windowSize = [AppleDetail CGSizeFromResolution:windowResolution];

        if( windowSize.width == 0 )
        {
            return false;
        }

        if( windowSize.height == 0 )
        {
            return false;
        }

        NSRect frame = NSMakeRect( 0, 0, windowSize.width, windowSize.height );

        m_controller = [[AppleWebViewController alloc] init];

        if( [m_controller initializeBrowser] == NO )
        {
            this->close();

            return false;
        }

        [m_controller setExternalNavigation:m_externalNavigation];

        NSView * contentView = m_controller.view;
        contentView.frame = frame;
        m_window = [[NSWindow alloc] initWithContentRect:frame styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable backing:NSBackingStoreBuffered defer:NO];
        m_window.releasedWhenClosed = NO;
        m_window.title = _title;
        m_window.contentViewController = m_controller;

        if( HAS_CONFIG( "Window", "MinSize" ) == true )
        {
            Resolution minimumResolution = CONFIG_VALUE_RESOLUTION( "Window", "MinSize", Resolution( 0, 0 ) );
            NSSize minimumSize = [AppleDetail CGSizeFromResolution:minimumResolution];
            m_window.contentMinSize = minimumSize;
        }

        if( _hidden == true )
        {
            CGFloat hiddenPosition = -10000;
            NSPoint origin = NSMakePoint( hiddenPosition, hiddenPosition );
            [m_window setFrameOrigin:origin];
            [m_window orderFront:nil];
        }
        else
        {
            [m_window center];
            [m_window makeKeyAndOrderFront:nil];
        }

        WebViewBridgeSenderInterfacePtr sender = WebViewBridgeSenderInterfacePtr::from( this );

        WEBVIEW_BRIDGE_SERVICE()
            ->setSender( sender );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    NSWindow * MacOSWebViewPlatformService::getNSWindow() const
    {
        return m_window;
    }
    //////////////////////////////////////////////////////////////////////////
    WKWebView * MacOSWebViewPlatformService::getWKWebView() const
    {
        WKWebView * browser = m_controller.m_webView;

        return browser;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSWebViewPlatformService::openContent( const FileGroupInterfacePtr & _fileGroup, const FilePath & _entry )
    {
        if( [m_controller openContent:_fileGroup entry:_entry] == NO )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSWebViewPlatformService::openURL( const String & _url )
    {
        NSString * url = [AppleString NSStringFromString:_url];

        if( [m_controller openURL:url] == NO )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSWebViewPlatformService::reload()
    {
        [m_controller reload];
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSWebViewPlatformService::goBack()
    {
        if( [m_controller goBack] == NO )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSWebViewPlatformService::goForward()
    {
        if( [m_controller goForward] == NO )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSWebViewPlatformService::sendJSON( const jpp::object & _message )
    {
        if( [m_controller sendJSON:_message] == NO )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object MacOSWebViewPlatformService::getCapabilities() const
    {
        jpp::object capabilities = jpp::make_object();
        capabilities.set( "snapshot", true );
        capabilities.set( "persistentStorage", true );
        capabilities.set( "localOrigin", "mengine://localhost" );

        return capabilities;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSWebViewPlatformService::takeSnapshot( const LambdaSnapshot & _callback )
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
    void MacOSWebViewPlatformService::setExternalNavigationCallback( const LambdaExternalNavigation & _callback )
    {
        m_externalNavigation = _callback;
        [m_controller setExternalNavigation:_callback];
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSWebViewPlatformService::close()
    {
        [m_controller close];
        m_controller = nil;

        m_window.delegate = nil;
        [m_window orderOut:nil];
        m_window.contentViewController = nil;
        [m_window close];
        m_window = nil;

        WEBVIEW_BRIDGE_SERVICE()
            ->setSender( nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSWebViewPlatformService::onWebViewBridgeSenderSendJSON( const jpp::object & _message )
    {
        if( this->sendJSON( _message ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
