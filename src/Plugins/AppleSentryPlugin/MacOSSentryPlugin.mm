#import "MacOSSentryPlugin.h"
#import "AppleSentryNotificationObserver.h"

#import "Environment/Apple/AppleBundle.h"

#include "Kernel/BuildMode.h"
#include "Kernel/OptionHelper.h"

#import <Sentry/Sentry.h>

@implementation MacOSSentryPlugin
{
    Mengine::AppleSentryNotificationObserver * m_notificationObserver;
}

#pragma mark - MacOSProxyApplicationDelegateInterface

static BOOL MacOSSentry_isAvailable() {
    if( HAS_OPTION( "sentry" ) == true )
    {
        return YES;
    }

    if( HAS_OPTION( "nosentry" ) == true )
    {
        return NO;
    }

    BOOL available = Mengine::Helper::AppleGetBundlePluginConfigBoolean( @("MengineMacOSSentryPlugin"), @("Available"), YES );

    return available;
}

- (instancetype _Nonnull)init {
    self = [super init];

    m_notificationObserver = nullptr;

    return self;
}

- (void)dealloc {
    if( m_notificationObserver != nullptr )
    {
        m_notificationObserver->finalize();
        delete m_notificationObserver;
        m_notificationObserver = nullptr;
    }
}

- (BOOL)application {
    if( MacOSSentry_isAvailable() == NO )
    {
        return YES;
    }

    NSString * MengineMacOSSentryPlugin_DSN = Mengine::Helper::AppleGetBundlePluginConfigString( @("MengineMacOSSentryPlugin"), @("DSN"), nil );
    
    if( MengineMacOSSentryPlugin_DSN == nil ) {
        return YES;
    }
    
    const Mengine::Char * BUILD_VERSION = Mengine::Helper::getBuildVersion();
    const Mengine::Char * BUILD_NUMBER_STRING = Mengine::Helper::getBuildNumberString();
    
#if defined(MENGINE_BUILD_PUBLISH_VALUE)
    const Mengine::Char * ENVIRONMENT = "production";
#else
    const Mengine::Char * ENVIRONMENT = "debug";
#endif
    
    [SentrySDK startWithConfigureOptions:^(SentryOptions *options) {
        options.dsn = MengineMacOSSentryPlugin_DSN;
        options.debug = NO;
        options.releaseName = @(BUILD_VERSION);
        options.dist = @(BUILD_NUMBER_STRING);
        options.environment = @(ENVIRONMENT);
    }];

    if( m_notificationObserver == nullptr )
    {
        m_notificationObserver = new Mengine::AppleSentryNotificationObserver();
        m_notificationObserver->initialize();
        m_notificationObserver->setupApplicationScope();
    }
    
    return YES;
}

@end
