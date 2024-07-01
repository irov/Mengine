#import "AppleSentryApplicationDelegate.h"

#import "Environment/Apple/AppleBundle.h"
#import "Environment/Apple/AppleUserDefaults.h"

#include "Kernel/BuildMode.h"

#import "Sentry/Sentry.h"

@implementation AppleSentryApplicationDelegate

#pragma mark - iOSPluginApplicationDelegateInterface

- (instancetype _Nonnull)init {
    self = [super init];
    
    self.m_sendAllow = NO;
    
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
    NSString * AppleSentryPlugin_DSN = Mengine::Helper::AppleGetBundlePluginConfigString( @("MengineAppleSentryPlugin"), @("DSN"), nil );
    
    if( AppleSentryPlugin_DSN == nil ) {
        return NO;
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
    
    return YES;
}

@end
