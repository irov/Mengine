#pragma once

#include "Interface/WebViewPlatformExtensionInterface.h"

#if defined(MENGINE_PLATFORM_MACOS)
#import <AppKit/AppKit.h>
#else
#import <UIKit/UIKit.h>
#endif
#import <WebKit/WebKit.h>

#if defined(MENGINE_PLATFORM_MACOS)
@interface AppleWebViewController : NSViewController<WKURLSchemeHandler, WKScriptMessageHandler, WKNavigationDelegate>
#else
@interface AppleWebViewController : UIViewController<WKURLSchemeHandler, WKScriptMessageHandler, WKNavigationDelegate>
#endif
{
@private
    Mengine::FileGroupInterfacePtr m_fileGroup;

    Mengine::WebViewPlatformExtensionInterface::LambdaExternalNavigation m_externalNavigation;
    Mengine::WebViewPlatformExtensionInterface::LambdaSnapshot m_snapshot;

    bool m_documentCommitted;
    uint64_t m_generation;
    uint64_t m_snapshotRevision;

    NSURL * m_allowedNavigationURL;
    WKNavigation * m_navigation;
}

@property (nonatomic, strong) WKWebView * m_webView;
#if defined(MENGINE_PLATFORM_MACOS)
@property (nonatomic, strong) NSView * m_overlay;
#else
@property (nonatomic, strong) UIView * m_overlay;
#endif

- (BOOL)initializeBrowser;
- (BOOL)openContent:(const Mengine::FileGroupInterfacePtr &)_fileGroup entry:(const Mengine::FilePath &)_entry;
- (BOOL)openURL:(NSString *)_url;
- (void)reload;
- (BOOL)goBack;
- (BOOL)goForward;
- (BOOL)sendJSON:(const jpp::object &)_message;
- (void)takeSnapshot:(const Mengine::WebViewPlatformExtensionInterface::LambdaSnapshot &)_callback;
- (void)setExternalNavigation:(const Mengine::WebViewPlatformExtensionInterface::LambdaExternalNavigation &)_callback;
- (void)close;

@end
