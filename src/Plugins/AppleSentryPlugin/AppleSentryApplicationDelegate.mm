#import "AppleSentryApplicationDelegate.h"
#import "AppleSentryNotificationObserver.h"

#import "Environment/Apple/AppleBundle.h"
#import "Environment/Apple/AppleUserDefaults.h"

#include "Kernel/BuildMode.h"
#include "Kernel/OptionHelper.h"

#import "Sentry/Sentry.h"

@implementation AppleSentryApplicationDelegate
{
    Mengine::AppleSentryNotificationObserver * m_notificationObserver;
}

#pragma mark - iOSPluginApplicationDelegateInterface

static BOOL AppleSentry_isAvailable() {
    if( HAS_OPTION( "sentry" ) == true )
    {
        return YES;
    }

    if( HAS_OPTION( "nosentry" ) == true )
    {
        return NO;
    }

    BOOL available = Mengine::Helper::AppleGetBundlePluginConfigBoolean( @("MengineAppleSentryPlugin"), @("Available"), YES );

    return available;
}

- (instancetype _Nonnull)init {
    self = [super init];
    
    self.m_sendAllow = NO;
    m_notificationObserver = nullptr;
    
    return self;
}

- (void)event:(NSString *)name args:(NSArray *)args {
    if( [name isEqualToString:@"NOTIFICATION_GDPR_PASS"] == true ) {
        BOOL passGDPR = [args[0] boolValue];
        
        if( passGDPR == YES ) {
            self.m_sendAllow = YES;
        } else {
            self.m_sendAllow = NO;
        }
    }
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    (void)application;
    (void)launchOptions;

    if( AppleSentry_isAvailable() == NO )
    {
        return YES;
    }

    NSString * AppleSentryPlugin_DSN = Mengine::Helper::AppleGetBundlePluginConfigString( @("MengineAppleSentryPlugin"), @("DSN"), nil );
    
    if( AppleSentryPlugin_DSN == nil ) {
        return YES;
    }
    
    BOOL passGDPR = Mengine::Helper::AppleGetUserDefaultsBoolean( @("mengine.gdpr.pass"), NO );
    
    if( passGDPR == YES ) {
        self.m_sendAllow = YES;
    } else {
        self.m_sendAllow = NO;
    }

    const Mengine::Char * BUILD_VERSION = Mengine::Helper::getBuildVersion();
    const Mengine::Char * BUILD_NUMBER_STRING = Mengine::Helper::getBuildNumberString();
    
#if defined(MENGINE_BUILD_PUBLISH_VALUE)
    const Mengine::Char * ENVIRONMENT = "production";
#else
    const Mengine::Char * ENVIRONMENT = "debug";
#endif
    
    [SentrySDK startWithConfigureOptions:^(SentryOptions *options) {
        options.dsn = AppleSentryPlugin_DSN;
        options.debug = NO;
        options.releaseName = @(BUILD_VERSION);
        options.dist = @(BUILD_NUMBER_STRING);
        options.environment = @(ENVIRONMENT);
        
        options.beforeSend = ^SentryEvent * _Nullable(SentryEvent * _Nonnull event) {
            if( self.m_sendAllow == NO ) {
                return nil;
            }
            
            return event;
        };
        
        options.beforeBreadcrumb = ^SentryBreadcrumb * _Nullable(SentryBreadcrumb * _Nonnull breadcrumb) {
            if( self.m_sendAllow == NO ) {
                return nil;
            }
            
            return breadcrumb;
        };
    }];

    if( m_notificationObserver == nullptr )
    {
        m_notificationObserver = new Mengine::AppleSentryNotificationObserver();
        m_notificationObserver->initialize();
        m_notificationObserver->setupApplicationScope();
    }
    
    return YES;
}

- (void)onFinalize {
    if( m_notificationObserver == nullptr )
    {
        return;
    }

    m_notificationObserver->finalize();
    delete m_notificationObserver;
    m_notificationObserver = nullptr;
}

@end
