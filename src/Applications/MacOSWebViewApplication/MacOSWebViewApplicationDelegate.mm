#import "MacOSWebViewApplicationDelegate.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/LifecycleServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/WebViewBootstrapperInterface.h"
#include "Interface/WebViewBridgeServiceInterface.h"
#include "Interface/WebViewPlatformExtensionInterface.h"
#include "Interface/WebViewPlatformServiceInterface.h"

#import "Platforms/AppleWebViewPlatform/AppleWebViewMCP.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleLogRecordParam.h"
#import "Environment/Apple/AppleString.h"
#include "Environment/MacOS/MacOSWebViewPlatformServiceExtensionInterface.h"

#include "Runtime/MengineRuntime.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePath.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/ProxyLogger.h"
#include "Kernel/StdioLogger.h"

#include "Config/StdString.h"

#import <WebKit/WebKit.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( WebViewPlatformService );
//////////////////////////////////////////////////////////////////////////
@implementation MacOSWebViewApplicationDelegate

#pragma mark - Application

- (BOOL)initializeApplication
{
    NSString * resourcePath = NSBundle.mainBundle.resourcePath;
    NSString * content = [resourcePath stringByAppendingPathComponent:@"WebView"];
    const Mengine::Char * contentPath = content.fileSystemRepresentation;

    Mengine::Configuration configuration;
    configuration.nopause = [NSProcessInfo.processInfo.arguments containsObject:@"--cli"] == YES;
    configuration.silentDialog = configuration.nopause;
    Mengine::StdString::strcpy_safe( configuration.dataDirectory, contentPath, MENGINE_MAX_PATH );

    if( API_MengineCreate( &configuration ) == nullptr )
    {
        return NO;
    }

    if( API_MengineBootstrap() == false )
    {
        API_MengineFinalize();

        return NO;
    }

    if( [self initializeLogger] == NO )
    {
        [self finalizeApplication];

        return NO;
    }

    Mengine::FilePath temporary = [AppleDetail getTemporaryDirectoryFilePath];

    if( FILE_SERVICE()
        ->mountFileGroup( STRINGIZE_STRING_LOCAL( "temporary" ), nullptr, nullptr, temporary, STRINGIZE_STRING_LOCAL( "global" ), nullptr, true, nullptr ) == false )
    {
        [self finalizeApplication];

        return NO;
    }

    const Mengine::FilePath & rootPath = Mengine::FilePath::none();

    if( FILE_SERVICE()
        ->mountFileGroup( STRINGIZE_STRING_LOCAL( "webview" ), nullptr, nullptr, rootPath, STRINGIZE_STRING_LOCAL( "dir" ), nullptr, false, nullptr ) == false )
    {
        [self finalizeApplication];

        return NO;
    }

    if( SERVICE_CREATE_SAFE( WebViewPlatformService, nullptr ) == false )
    {
        [self finalizeApplication];

        return NO;
    }

    if( API_MengineRun() == false )
    {
        [self finalizeApplication];

        return NO;
    }

    return YES;
}

- (void)finalizeApplication
{
    [m_mcp stop];
    m_mcp = nil;

    [self pauseRuntime];

    if( SERVICE_PROVIDER_EXIST() == false )
    {
        return;
    }

    if( SERVICE_IS_EXIST( Mengine::WebViewPlatformServiceInterface ) == true )
    {
        SERVICE_FINALIZE( WebViewPlatformService );
        SERVICE_DESTROY( WebViewPlatformService );
    }

    if( SERVICE_IS_INITIALIZE( Mengine::WebViewBootstrapperInterface ) == true )
    {
        WEBVIEW_BOOTSTRAPPER_SERVICE()
            ->stop();
    }

    if( SERVICE_IS_INITIALIZE( Mengine::FileServiceInterface ) == true )
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

    if( m_logCollector != nullptr )
    {
        LOGGER_SERVICE()
            ->unregisterLogger( m_logCollector );

        m_logCollector = nullptr;
    }

    m_records = nil;

    if( m_logger != nullptr )
    {
        LOGGER_SERVICE()
            ->unregisterLogger( m_logger );

        m_logger = nullptr;
    }

    API_MengineFinalize();
}

- (void)stopApplication
{
    [self finalizeApplication];
    [NSApp terminate:nil];
}

#pragma mark - Runtime

- (void)resumeRuntime
{
    NSTimer * runtimeTimer = self.m_runtimeTimer;

    if( runtimeTimer != nil )
    {
        return;
    }

    if( SERVICE_PROVIDER_EXIST() == false )
    {
        return;
    }

    self.m_previousTime = NSProcessInfo.processInfo.systemUptime;

    __weak MacOSWebViewApplicationDelegate * weakSelf = self;
    NSTimeInterval runtimeInterval = 0.01;
    NSTimer * timer = [NSTimer timerWithTimeInterval:runtimeInterval repeats:YES block:^(NSTimer * _timer)
    {
        MENGINE_UNUSED( _timer );

        MacOSWebViewApplicationDelegate * delegate = weakSelf;

        if( delegate == nil )
        {
            return;
        }

        if( SERVICE_PROVIDER_EXIST() == false )
        {
            return;
        }

        CFTimeInterval now = NSProcessInfo.processInfo.systemUptime;
        float milliseconds = static_cast<float>((now - delegate.m_previousTime) * 1000.0);
        delegate.m_previousTime = now;

        WEBVIEW_BOOTSTRAPPER_SERVICE()
            ->tick( milliseconds );
    }];

    [NSRunLoop.mainRunLoop addTimer:timer forMode:NSRunLoopCommonModes];
    self.m_runtimeTimer = timer;
}

- (void)pauseRuntime
{
    NSTimer * timer = self.m_runtimeTimer;
    [timer invalidate];
    self.m_runtimeTimer = nil;
}

#pragma mark - WebView

- (WKWebView *)browser
{
    if( SERVICE_PROVIDER_EXIST() == false )
    {
        return nil;
    }

    Mengine::MacOSWebViewPlatformServiceExtensionInterface * platform = WEBVIEW_PLATFORM_SERVICE()
        ->getUnknown();
    WKWebView * browser = platform->getWKWebView();

    return browser;
}

- (NSDictionary *)webViewStatus
{
    bool documentReady = WEBVIEW_BRIDGE_SERVICE()
        ->isDocumentReady();
    bool appReady = WEBVIEW_BRIDGE_SERVICE()
        ->isAppReady();
    uint64_t generation = WEBVIEW_BRIDGE_SERVICE()
        ->getDocumentGeneration();
    NSString * documentGeneration = [NSString stringWithFormat:@"%llu", generation];

    WKWebView * browser = [self browser];
    BOOL runtimeTimer = self.m_runtimeTimer.valid;
    NSString * url = browser.URL.absoluteString ?: @"";

    NSDictionary * status = @{
        @"documentReady": @(documentReady),
        @"appReady": @(appReady),
        @"generation": documentGeneration,
        @"runtimeTimer": @(runtimeTimer),
        @"url": url
    };

    return status;
}

- (void)reloadWebView
{
    Mengine::WebViewPlatformExtensionInterface * browser = WEBVIEW_PLATFORM_SERVICE()
        ->getUnknown();

    browser->reload();
}

- (void)captureWebView:(void (^)(NSData *, uint32_t, uint32_t))_completion
{
    Mengine::WebViewPlatformExtensionInterface * browser = WEBVIEW_PLATFORM_SERVICE()
        ->getUnknown();

    browser->takeSnapshot( [_completion]( const Mengine::Data & _data, uint32_t _width, uint32_t _height )
    {
        const uint8_t * buffer = _data.data();
        size_t size = _data.size();

        NSData * data = [NSData dataWithBytes:buffer length:size];

        _completion( data, _width, _height );
    } );
}

- (void)evaluateWebView:(NSString *)_script completion:(void (^)(id, NSError *))_completion
{
    WKWebView * browser = [self browser];
    [browser evaluateJavaScript:_script completionHandler:_completion];
}

#pragma mark - Logging

- (BOOL)initializeLogger
{
    Mengine::LoggerInterfacePtr logger = Mengine::Helper::makeFactorableUnique<Mengine::StdioLogger>( nullptr );
    logger->setWriteHistory( true );

    if( LOGGER_SERVICE()
        ->registerLogger( logger ) == false )
    {
        return NO;
    }

    m_logger = logger;

    Class mcpClass = NSClassFromString( @"AppleWebViewMCP" );

    if( mcpClass == nil )
    {
        return YES;
    }

    NSMutableArray * records = [[NSMutableArray alloc] init];
    Mengine::ProxyLoggerPtr collector = Mengine::Helper::makeFactorableUnique<Mengine::ProxyLogger>( nullptr );
    collector->setLambda( [records]( const Mengine::LoggerRecordInterfacePtr & _record )
    {
        Mengine::LoggerMessage message;
        _record->getMessage( &message );

        const Mengine::Char * categoryValue = message.category != nullptr ? message.category : "";
        size_t categorySize = Mengine::StdString::strlen( categoryValue );
        NSString * category = [AppleString NSStringFromString:categoryValue withSize:categorySize] ?: @"";
        NSString * text = [AppleString NSStringFromString:message.data withSize:message.size] ?: @"";
        @synchronized( records )
        {
            NSDictionary * previous = records.lastObject;
            NSNumber * previousSequence = previous[@"sequence"];
            uint64_t sequence = previousSequence.unsignedLongLongValue + 1;
            NSDictionary * record = @{
                @"sequence": @(sequence),
                @"timestamp": @(message.timestamp),
                @"level": @(message.level),
                @"category": category,
                @"message": text
            };

            if( records.count >= 5000 )
            {
                [records removeObjectAtIndex:0];
            }

            [records addObject:record];
        }
    } );
    collector->setWriteHistory( true );

    if( LOGGER_SERVICE()
        ->registerLogger( collector ) == false )
    {
        return NO;
    }

    m_logCollector = collector;
    m_records = records;

    return YES;
}

- (NSArray *)readLogs
{
    NSMutableArray * records = m_records;

    if( records == nil )
    {
        return @[];
    }

    @synchronized( records )
    {
        NSArray * result = [records copy];

        return result;
    }
}

- (void)eventLog:(AppleLogRecordParam *)_record
{
    if( SERVICE_PROVIDER_EXIST() == false )
    {
        return;
    }

    if( SERVICE_IS_INITIALIZE( Mengine::LoggerServiceInterface ) == false )
    {
        return;
    }

    NSString * threadName = _record.LOG_THREAD ?: @"Unknown";
    Mengine::ConstString thread = [AppleString NSStringToConstString:threadName];

    Mengine::LoggerMessage message;
    message.timestamp = _record.LOG_TIMESTAMP;
    message.category = _record.LOG_CATEGORY.UTF8String;
    message.thread = thread;
    message.level = _record.LOG_LEVEL;
    message.flag = _record.LOG_FLAG;
    message.filter = _record.LOG_FILTER;
    message.color = _record.LOG_COLOR;
    message.file = _record.LOG_FILE.UTF8String;
    message.line = _record.LOG_LINE;
    message.function = _record.LOG_FUNCTION.UTF8String;
    message.data = _record.LOG_DATA.UTF8String;
    message.size = Mengine::StdString::strlen( message.data );

    LOGGER_SERVICE()
        ->logMessage( message );
}

#pragma mark - NSApplicationDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)_notification
{
    MENGINE_UNUSED( _notification );

    if( [self initializeApplication] == NO )
    {
        [self stopApplication];

        return;
    }

    NSString * title = [NSBundle.mainBundle objectForInfoDictionaryKey:@"CFBundleDisplayName"] ?: @"Mengine WebView";
    bool hidden = [NSProcessInfo.processInfo.arguments containsObject:@"--windowhidden"] == YES;

    Mengine::MacOSWebViewPlatformServiceExtensionInterface * platform = WEBVIEW_PLATFORM_SERVICE()
        ->getUnknown();

    if( platform->createWindow( title, hidden ) == false )
    {
        [self stopApplication];

        return;
    }

    NSWindow * window = platform->getNSWindow();
    window.delegate = self;

    NSString * entry = [NSBundle.mainBundle objectForInfoDictionaryKey:@"MengineWebViewEntry"] ?: @"index.html";
    Mengine::ConstString entryString = [AppleString NSStringToConstString:entry];
    Mengine::FilePath entryPath( entryString );

    const Mengine::FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
        ->getFileGroup( STRINGIZE_STRING_LOCAL( "webview" ) );

    if( platform->openContent( fileGroup, entryPath ) == false )
    {
        [self stopApplication];

        return;
    }

    [self resumeRuntime];

    Class mcpClass = NSClassFromString( @"AppleWebViewMCP" );
    m_mcp = [[mcpClass alloc] init];
    [m_mcp startWithDelegate:self];
}

- (void)applicationDidBecomeActive:(NSNotification *)_notification
{
    MENGINE_UNUSED( _notification );

    if( SERVICE_PROVIDER_EXIST() == false )
    {
        return;
    }

    LIFECYCLE_SERVICE()
        ->onApplicationDidBecomeActive();
}

- (void)applicationWillResignActive:(NSNotification *)_notification
{
    MENGINE_UNUSED( _notification );

    if( SERVICE_PROVIDER_EXIST() == false )
    {
        return;
    }

    LIFECYCLE_SERVICE()
        ->onApplicationWillResignActive();
}

- (void)applicationWillTerminate:(NSNotification *)_notification
{
    MENGINE_UNUSED( _notification );

    if( SERVICE_PROVIDER_EXIST() == false )
    {
        [self finalizeApplication];

        return;
    }

    LIFECYCLE_SERVICE()
        ->onApplicationWillTerminate();

    [self finalizeApplication];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)_application
{
    MENGINE_UNUSED( _application );

    return YES;
}

#pragma mark - NSWindowDelegate

- (void)windowWillClose:(NSNotification *)_notification
{
    MENGINE_UNUSED( _notification );

    [NSApp terminate:nil];
}

@end
