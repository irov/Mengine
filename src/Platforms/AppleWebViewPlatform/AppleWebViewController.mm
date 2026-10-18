#import "AppleWebViewController.h"

#import "Environment/Apple/AppleFile.h"
#import "Environment/Apple/AppleString.h"

#if defined(MENGINE_PLATFORM_MACOS)
#   import "Environment/Apple/AppleLog.h"
#else
#   import "Environment/iOS/iOSLog.h"
#endif

#include "Interface/WebViewBridgeServiceInterface.h"

#include "Kernel/FilePath.h"
#include "Kernel/JSONHelper.h"

#include "Config/StdIntTypes.h"

//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_MACOS)
typedef NSView AppleWebViewView;
typedef NSImage AppleWebViewSnapshotImage;
#else
typedef UIView AppleWebViewView;
typedef UIImage AppleWebViewSnapshotImage;
#endif
//////////////////////////////////////////////////////////////////////////

@implementation AppleWebViewController

#pragma mark - Layout

+ (void)addPinnedSubview:(AppleWebViewView *)_view toParent:(AppleWebViewView *)_parent
{
    _view.translatesAutoresizingMaskIntoConstraints = NO;
    [_parent addSubview:_view];

    NSLayoutConstraint * leading = [NSLayoutConstraint constraintWithItem:_view attribute:NSLayoutAttributeLeading relatedBy:NSLayoutRelationEqual toItem:_parent attribute:NSLayoutAttributeLeading multiplier:1.0 constant:0.0];
    NSLayoutConstraint * trailing = [NSLayoutConstraint constraintWithItem:_view attribute:NSLayoutAttributeTrailing relatedBy:NSLayoutRelationEqual toItem:_parent attribute:NSLayoutAttributeTrailing multiplier:1.0 constant:0.0];
    NSLayoutConstraint * top = [NSLayoutConstraint constraintWithItem:_view attribute:NSLayoutAttributeTop relatedBy:NSLayoutRelationEqual toItem:_parent attribute:NSLayoutAttributeTop multiplier:1.0 constant:0.0];
    NSLayoutConstraint * bottom = [NSLayoutConstraint constraintWithItem:_view attribute:NSLayoutAttributeBottom relatedBy:NSLayoutRelationEqual toItem:_parent attribute:NSLayoutAttributeBottom multiplier:1.0 constant:0.0];
    NSArray<NSLayoutConstraint *> * constraints = @[leading, trailing, top, bottom];

    [NSLayoutConstraint activateConstraints:constraints];
}

#pragma mark - Lifecycle

- (BOOL)initializeBrowser
{
    NSURL * scriptURL = [NSBundle.mainBundle URLForResource:@"MengineWebViewBridge" withExtension:@"js"];
    NSError * error = nil;
    NSString * script = [NSString stringWithContentsOfURL:scriptURL encoding:NSUTF8StringEncoding error:&error];

    if( script == nil )
    {
#if defined(MENGINE_PLATFORM_MACOS)
        [AppleLog withFormat:@"WebView bridge script is unavailable"];
#else
        IOS_LOGGER_ERROR( @"WebView bridge script is unavailable" );
#endif

        return NO;
    }

    WKWebViewConfiguration * configuration = [[WKWebViewConfiguration alloc] init];
    configuration.websiteDataStore = [WKWebsiteDataStore defaultDataStore];
    [configuration setURLSchemeHandler:self forURLScheme:@"mengine"];

    WKUserContentController * userContentController = configuration.userContentController;
    [userContentController addScriptMessageHandler:self name:@"mengine"];

    WKUserScript * userScript = [[WKUserScript alloc] initWithSource:script injectionTime:WKUserScriptInjectionTimeAtDocumentStart forMainFrameOnly:YES];
    [userContentController addUserScript:userScript];

    WKWebView * browser = [[WKWebView alloc] initWithFrame:CGRectZero configuration:configuration];
    browser.navigationDelegate = self;
    self.m_webView = browser;

#if defined(MENGINE_PLATFORM_MACOS)
    NSView * contentView = [[NSView alloc] initWithFrame:NSZeroRect];
#else
    UIView * contentView = [[UIView alloc] initWithFrame:CGRectZero];
    contentView.backgroundColor = UIColor.systemBackgroundColor;

    UIScrollView * scrollView = browser.scrollView;
    scrollView.contentInsetAdjustmentBehavior = UIScrollViewContentInsetAdjustmentNever;
#endif

    self.view = contentView;
    [AppleWebViewController addPinnedSubview:browser toParent:contentView];

#if defined(MENGINE_PLATFORM_MACOS)
    NSRect overlayFrame = contentView.bounds;
    NSView * overlay = [[NSView alloc] initWithFrame:overlayFrame];
    overlay.wantsLayer = YES;
    overlay.layer.backgroundColor = NSColor.windowBackgroundColor.CGColor;
    overlay.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    [contentView addSubview:overlay];

    CGFloat activitySize = 32;
    NSRect activityFrame = NSMakeRect( 0, 0, activitySize, activitySize );
    NSProgressIndicator * activity = [[NSProgressIndicator alloc] initWithFrame:activityFrame];
    activity.style = NSProgressIndicatorStyleSpinning;
#else
    UIView * overlay = [[UIView alloc] initWithFrame:CGRectZero];
    overlay.backgroundColor = UIColor.systemBackgroundColor;
    [AppleWebViewController addPinnedSubview:overlay toParent:contentView];

    UIActivityIndicatorView * activity = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleLarge];
#endif

    self.m_overlay = overlay;

    activity.translatesAutoresizingMaskIntoConstraints = NO;
    [overlay addSubview:activity];

    NSLayoutConstraint * centerX = [NSLayoutConstraint constraintWithItem:activity attribute:NSLayoutAttributeCenterX relatedBy:NSLayoutRelationEqual toItem:overlay attribute:NSLayoutAttributeCenterX multiplier:1.0 constant:0.0];
    NSLayoutConstraint * centerY = [NSLayoutConstraint constraintWithItem:activity attribute:NSLayoutAttributeCenterY relatedBy:NSLayoutRelationEqual toItem:overlay attribute:NSLayoutAttributeCenterY multiplier:1.0 constant:0.0];
    NSArray<NSLayoutConstraint *> * activityConstraints = @[centerX, centerY];

    [NSLayoutConstraint activateConstraints:activityConstraints];

#if defined(MENGINE_PLATFORM_MACOS)
    [activity startAnimation:nil];
#else
    [activity startAnimating];
#endif

    m_documentCommitted = false;
    m_generation = 0;
    m_snapshotRevision = 0;

    return YES;
}

- (void)close
{
    m_documentCommitted = false;

    WEBVIEW_BRIDGE_SERVICE()
        ->closeDocument();

    m_externalNavigation = nullptr;
    m_allowedNavigationURL = nil;
    m_navigation = nil;

    WKWebView * browser = self.m_webView;
    [browser stopLoading];
    browser.navigationDelegate = nil;

    WKUserContentController * userContentController = browser.configuration.userContentController;
    [userContentController removeScriptMessageHandlerForName:@"mengine"];

    [browser removeFromSuperview];
    self.m_webView = nil;
    m_fileGroup = nullptr;

    [self cancelSnapshot];
}

#pragma mark - Navigation

- (BOOL)openContent:(const Mengine::FileGroupInterfacePtr &)_fileGroup entry:(const Mengine::FilePath &)_entry
{
    if( _fileGroup == nullptr )
    {
        return NO;
    }

    if( _entry.empty() == true )
    {
        return NO;
    }

    if( _fileGroup->existFile( _entry, false ) == false )
    {
        return NO;
    }

    m_fileGroup = _fileGroup;

    NSURLComponents * components = [[NSURLComponents alloc] init];
    components.scheme = @"mengine";
    components.host = @"localhost";

    NSString * entry = [AppleString NSStringFromConstString:_entry];
    components.path = [@"/" stringByAppendingString:entry];

    NSURL * url = components.URL;

    if( url == nil )
    {
        return NO;
    }

    NSURLRequest * request = [NSURLRequest requestWithURL:url];
    m_allowedNavigationURL = url;
    WKWebView * browser = self.m_webView;
    [browser loadRequest:request];

    return YES;
}

- (BOOL)openURL:(NSString *)_url
{
    NSURL * url = [NSURL URLWithString:_url];
    NSString * scheme = url.scheme;

    if( [scheme isEqualToString:@"https"] == NO )
    {
        return NO;
    }

    NSUInteger hostLength = url.host.length;

    if( hostLength == 0 )
    {
        return NO;
    }

    NSString * user = url.user;

    if( user != nil )
    {
        return NO;
    }

    NSString * password = url.password;

    if( password != nil )
    {
        return NO;
    }

    NSURLRequest * request = [NSURLRequest requestWithURL:url];
    m_allowedNavigationURL = url;
    WKWebView * browser = self.m_webView;
    [browser loadRequest:request];

    return YES;
}

- (void)reload
{
    WKWebView * browser = self.m_webView;
    m_allowedNavigationURL = browser.URL;
    [browser reload];
}

- (BOOL)goBack
{
    WKWebView * browser = self.m_webView;
    BOOL canGoBack = browser.canGoBack;

    if( canGoBack == NO )
    {
        return NO;
    }

    m_allowedNavigationURL = browser.backForwardList.backItem.URL;
    [browser goBack];

    return YES;
}

- (BOOL)goForward
{
    WKWebView * browser = self.m_webView;
    BOOL canGoForward = browser.canGoForward;

    if( canGoForward == NO )
    {
        return NO;
    }

    m_allowedNavigationURL = browser.backForwardList.forwardItem.URL;
    [browser goForward];

    return YES;
}

- (void)setExternalNavigation:(const Mengine::WebViewPlatformExtensionInterface::LambdaExternalNavigation &)_callback
{
    m_externalNavigation = _callback;
}

#pragma mark - URL validation

- (BOOL)isTrustedURL:(NSURL *)_url
{
    NSString * scheme = _url.scheme;

    if( [scheme isEqualToString:@"mengine"] == NO )
    {
        return NO;
    }

    NSString * host = _url.host;

    if( [host isEqualToString:@"localhost"] == NO )
    {
        return NO;
    }

    NSNumber * port = _url.port;

    if( port != nil )
    {
        return NO;
    }

    NSString * user = _url.user;

    if( user != nil )
    {
        return NO;
    }

    NSString * password = _url.password;

    if( password != nil )
    {
        return NO;
    }

    return YES;
}

- (BOOL)isReadableContentURL:(NSURL *)_url
{
    if( [self isTrustedURL:_url] == NO )
    {
        return NO;
    }

    NSString * path = _url.path;

    if( [path hasPrefix:@"/"] == NO )
    {
        return NO;
    }

    NSArray<NSString *> * components = path.pathComponents;

    if( [components containsObject:@".."] == YES )
    {
        return NO;
    }

    if( [path containsString:@"\\"] == YES )
    {
        return NO;
    }

    unichar nullCharacterCode = 0;
    NSString * nullCharacter = [NSString stringWithFormat:@"%C", nullCharacterCode];
    NSRange nullRange = [path rangeOfString:nullCharacter];

    if( nullRange.location != NSNotFound )
    {
        return NO;
    }

    return YES;
}

#pragma mark - Bridge

- (void)connectWebViewBridge
{
    if( m_documentCommitted == false )
    {
        return;
    }

    WKWebView * browser = self.m_webView;
    NSString * script = [NSString stringWithFormat:@"window.Mengine.connect('%llu')", m_generation];
    [browser evaluateJavaScript:script completionHandler:nil];
}

- (BOOL)sendJSON:(const jpp::object &)_message
{
    bool ready = WEBVIEW_BRIDGE_SERVICE()
        ->isDocumentReady();

    if( ready == false )
    {
        return NO;
    }

    WKWebView * browser = self.m_webView;

    if( browser == nil )
    {
        return NO;
    }

    NSURL * url = browser.URL;

    if( [self isTrustedURL:url] == NO )
    {
        return NO;
    }

    if( _message.is_type_object() == false )
    {
        return NO;
    }

    Mengine::String json;
    if( Mengine::Helper::writeJSONStringCompact( _message, &json ) == false )
    {
        return NO;
    }

    const Mengine::Char * buffer = json.data();
    size_t size = json.size();
    NSData * data = [NSData dataWithBytes:buffer length:size];
    id object = [NSJSONSerialization JSONObjectWithData:data options:0 error:nil];

    if( [object isKindOfClass:NSDictionary.class] == NO )
    {
        return NO;
    }

    NSMutableDictionary * message = [object mutableCopy];
    message[@"generation"] = [NSString stringWithFormat:@"%llu", m_generation];

    NSDictionary * arguments = @{@"message": message};
    WKContentWorld * contentWorld = WKContentWorld.pageWorld;

    [browser callAsyncJavaScript:@"window.Mengine.receive(message)" arguments:arguments inFrame:nil inContentWorld:contentWorld completionHandler:nil];

    return YES;
}

#pragma mark - Snapshots

- (void)takeSnapshot:(const Mengine::WebViewPlatformExtensionInterface::LambdaSnapshot &)_callback
{
    WKWebView * browser = self.m_webView;

    if( m_snapshot != nullptr )
    {
        Mengine::Data data;
        _callback( data, 0, 0 );

        return;
    }

    if( browser == nil )
    {
        Mengine::Data data;
        _callback( data, 0, 0 );

        return;
    }

    m_snapshot = _callback;
    uint64_t revision = ++m_snapshotRevision;

    __weak AppleWebViewController * weakSelf = self;
    [browser takeSnapshotWithConfiguration:nil completionHandler:^(AppleWebViewSnapshotImage * _image, NSError * _error)
    {
        MENGINE_UNUSED( _error );

        AppleWebViewController * controller = weakSelf;

        if( controller == nil )
        {
            return;
        }

        if( controller->m_snapshotRevision != revision )
        {
            return;
        }

        if( controller->m_snapshot == nullptr )
        {
            return;
        }

        Mengine::WebViewPlatformExtensionInterface::LambdaSnapshot callback = controller->m_snapshot;
        controller->m_snapshot = nullptr;

        Mengine::Data data;

        if( _image == nil )
        {
            callback( data, 0, 0 );

            return;
        }

#if defined(MENGINE_PLATFORM_MACOS)
        NSData * tiff = _image.TIFFRepresentation;
        NSBitmapImageRep * bitmap = [NSBitmapImageRep imageRepWithData:tiff];
        NSDictionary * properties = @{};
        NSData * png = [bitmap representationUsingType:NSBitmapImageFileTypePNG properties:properties];
        uint32_t width = static_cast<uint32_t>(bitmap.pixelsWide);
        uint32_t height = static_cast<uint32_t>(bitmap.pixelsHigh);
#else
        NSData * png = UIImagePNGRepresentation( _image );
        uint32_t width = static_cast<uint32_t>(_image.size.width * _image.scale);
        uint32_t height = static_cast<uint32_t>(_image.size.height * _image.scale);
#endif
        if( png == nil )
        {
            callback( data, width, height );

            return;
        }

        const uint8_t * bytes = static_cast<const uint8_t *>(png.bytes);
        NSUInteger size = png.length;
        const uint8_t * end = bytes + size;
        data.assign( bytes, end );

        callback( data, width, height );
    }];
}

- (void)cancelSnapshot
{
    ++m_snapshotRevision;

    if( m_snapshot == nullptr )
    {
        return;
    }

    Mengine::WebViewPlatformExtensionInterface::LambdaSnapshot callback = m_snapshot;
    m_snapshot = nullptr;

    Mengine::Data data;
    callback( data, 0, 0 );
}

#pragma mark - WKURLSchemeHandler

- (void)webView:(WKWebView *)_webView startURLSchemeTask:(id<WKURLSchemeTask>)_task
{
    MENGINE_UNUSED( _webView );

    NSURL * url = _task.request.URL;
    NSString * path = url.path;
    BOOL valid = [self isReadableContentURL:url];

    if( valid == NO )
    {
        NSError * error = [NSError errorWithDomain:NSURLErrorDomain code:NSURLErrorNoPermissionsToReadFile userInfo:nil];
        [_task didFailWithError:error];

        return;
    }

    if( m_fileGroup == nullptr )
    {
        NSError * error = [NSError errorWithDomain:NSURLErrorDomain code:NSURLErrorNoPermissionsToReadFile userInfo:nil];
        [_task didFailWithError:error];

        return;
    }

    NSString * relativePath = [path substringFromIndex:1];
    Mengine::ConstString relativePathString = [AppleString NSStringToConstString:relativePath];
    Mengine::FilePath filePath( relativePathString );

    if( m_fileGroup->existFile( filePath, false ) == false )
    {
        NSError * error = [NSError errorWithDomain:NSURLErrorDomain code:NSURLErrorFileDoesNotExist userInfo:nil];
        [_task didFailWithError:error];

        return;
    }

    NSError * error = nil;
    NSData * data = [AppleFile getNSDataFromFileGroup:m_fileGroup filePath:filePath error:&error];

    if( data == nil )
    {
        [_task didFailWithError:error];

        return;
    }

    NSDictionary<NSString *, NSString *> * mimeTypes = @{
        @"html": @"text/html",
        @"js": @"text/javascript",
        @"mjs": @"text/javascript",
        @"css": @"text/css",
        @"json": @"application/json",
        @"svg": @"image/svg+xml",
        @"png": @"image/png",
        @"jpg": @"image/jpeg",
        @"jpeg": @"image/jpeg",
        @"webp": @"image/webp",
        @"avif": @"image/avif",
        @"woff2": @"font/woff2",
        @"wasm": @"application/wasm",
        @"mp3": @"audio/mpeg",
        @"ogg": @"audio/ogg",
        @"wav": @"audio/wav",
        @"mp4": @"video/mp4",
        @"ico": @"image/x-icon"
    };

    NSString * extension = path.pathExtension.lowercaseString;
    NSString * mime = mimeTypes[extension] ?: @"application/octet-stream";

    NSDictionary * headers = @{
        @"Content-Type": mime,
        @"Access-Control-Allow-Origin": @"mengine://localhost",
        @"X-Content-Type-Options": @"nosniff"
    };

    NSHTTPURLResponse * response = [[NSHTTPURLResponse alloc] initWithURL:url statusCode:200 HTTPVersion:@"HTTP/1.1" headerFields:headers];

    [_task didReceiveResponse:response];
    [_task didReceiveData:data];
    [_task didFinish];
}

- (void)webView:(WKWebView *)_webView stopURLSchemeTask:(id<WKURLSchemeTask>)_task
{
    MENGINE_UNUSED( _webView );
    MENGINE_UNUSED( _task );

    //Empty
}

#pragma mark - WKScriptMessageHandler

- (void)userContentController:(WKUserContentController *)_controller didReceiveScriptMessage:(WKScriptMessage *)_message
{
    MENGINE_UNUSED( _controller );

    if( m_documentCommitted == false )
    {
        return;
    }

    WKFrameInfo * frame = _message.frameInfo;
    BOOL mainFrame = frame.mainFrame;

    if( mainFrame == NO )
    {
        return;
    }

    NSURL * url = frame.request.URL;

    if( [self isTrustedURL:url] == NO )
    {
        return;
    }

    id messageBody = _message.body;

    if( [messageBody isKindOfClass:NSString.class] == NO )
    {
        return;
    }

    NSString * json = messageBody;
    NSData * data = [json dataUsingEncoding:NSUTF8StringEncoding];

    if( data == nil )
    {
        return;
    }

    NSUInteger maximumMessageSize = 1024 * 1024;
    NSUInteger size = data.length;

    if( size > maximumMessageSize )
    {
        return;
    }

    id object = [NSJSONSerialization JSONObjectWithData:data options:0 error:nil];

    if( [object isKindOfClass:NSDictionary.class] == NO )
    {
        return;
    }

    NSDictionary * body = object;
    NSString * kind = body[@"kind"];

    if( [kind isKindOfClass:NSString.class] == NO )
    {
        return;
    }

    if( [kind isEqualToString:@"connect"] == YES )
    {
        [self connectWebViewBridge];

        return;
    }

    NSString * generation = body[@"generation"];

    if( [generation isKindOfClass:NSString.class] == NO )
    {
        return;
    }

    NSString * expected = [NSString stringWithFormat:@"%llu", m_generation];

    if( [generation isEqualToString:expected] == NO )
    {
        return;
    }

    const void * buffer = data.bytes;
    jpp::object message = Mengine::Helper::loadJSONBuffer( buffer, size, nullptr );

    WEBVIEW_BRIDGE_SERVICE()
        ->receiveMessage( m_generation, message );

    if( WEBVIEW_BRIDGE_SERVICE()
        ->isAppReady() == false )
    {
        self.m_overlay.hidden = NO;
    }
    else
    {
        self.m_overlay.hidden = YES;
    }
}

#pragma mark - WKNavigationDelegate

- (void)webView:(WKWebView *)_webView didStartProvisionalNavigation:(WKNavigation *)_navigation
{
    MENGINE_UNUSED( _webView );

    m_documentCommitted = false;
    m_navigation = _navigation;
    [self cancelSnapshot];

    m_generation = WEBVIEW_BRIDGE_SERVICE()
        ->beginDocument();

    self.m_overlay.hidden = NO;
}

- (void)webView:(WKWebView *)_webView didCommitNavigation:(WKNavigation *)_navigation
{
    if( _navigation != m_navigation )
    {
        return;
    }

    NSURL * url = _webView.URL;

    if( [self isTrustedURL:url] == NO )
    {
        WEBVIEW_BRIDGE_SERVICE()
            ->closeDocument();

        self.m_overlay.hidden = YES;

        return;
    }

    m_documentCommitted = true;
    [self connectWebViewBridge];
}

- (void)webView:(WKWebView *)_webView decidePolicyForNavigationAction:(WKNavigationAction *)_action decisionHandler:(void (^)(WKNavigationActionPolicy))_decisionHandler
{
    MENGINE_UNUSED( _webView );

    NSURL * url = _action.request.URL;
    BOOL local = [self isTrustedURL:url];
    NSString * scheme = url.scheme;
    BOOL https = [scheme isEqualToString:@"https"];
    BOOL allowedNavigation = [url isEqual:m_allowedNavigationURL];
    BOOL explicitHTTPS = https == YES && allowedNavigation == YES;
    WKFrameInfo * targetFrame = _action.targetFrame;
    BOOL mainFrame = targetFrame.mainFrame;

    if( mainFrame == YES && (local == YES || explicitHTTPS == YES) )
    {
        m_allowedNavigationURL = nil;
        _decisionHandler( WKNavigationActionPolicyAllow );

        return;
    }

    _decisionHandler( WKNavigationActionPolicyCancel );

    WKNavigationType navigationType = _action.navigationType;

    if( navigationType != WKNavigationTypeLinkActivated )
    {
        return;
    }

    if( targetFrame != nil && mainFrame == NO )
    {
        return;
    }

    if( m_externalNavigation == nullptr )
    {
        return;
    }

    if( url == nil )
    {
        return;
    }

    NSString * urlString = url.absoluteString;
    Mengine::String value = [AppleString NSStringToString:urlString];
    Mengine::WebViewPlatformExtensionInterface::LambdaExternalNavigation callback = m_externalNavigation;
    callback( value );
}

- (void)webView:(WKWebView *)_webView didFailProvisionalNavigation:(WKNavigation *)_navigation withError:(NSError *)_error
{
    MENGINE_UNUSED( _webView );

    if( _navigation != m_navigation )
    {
        return;
    }

    m_documentCommitted = false;

    WEBVIEW_BRIDGE_SERVICE()
        ->closeDocument();

#if defined(MENGINE_PLATFORM_MACOS)
    [AppleLog withFormat:@"WebView navigation failed: %@"
        , _error.localizedDescription
    ];
#else
    IOS_LOGGER_ERROR( @"WebView navigation failed: %@"
        , _error.localizedDescription
    );
#endif
}

- (void)webView:(WKWebView *)_webView didFailNavigation:(WKNavigation *)_navigation withError:(NSError *)_error
{
    [self webView:_webView didFailProvisionalNavigation:_navigation withError:_error];
}

- (void)webViewWebContentProcessDidTerminate:(WKWebView *)_webView
{
    MENGINE_UNUSED( _webView );

    m_documentCommitted = false;

    WEBVIEW_BRIDGE_SERVICE()
        ->closeDocument();

    [self reload];
}

@end
