#import "AppleSentryApplicationDelegate.h"

#import "Environment/Apple/AppleDetail.h"

#include "Kernel/BuildMode.h"

#import "Sentry/Sentry.h"

@implementation AppleSentryApplicationDelegate

#pragma mark - UIKitProxyApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    NSString * AppleSentryPlugin_DSN = Mengine::Helper::AppleGetBundlePluginConfigString( @("MengineAppleSentryPlugin"), @("DSN"), nil );
    
    if( AppleSentryPlugin_DSN == nil ) {
        return NO;
    }
    
    BOOL AppleSentryPlugin_Debug = MENGINE_DEBUG_VALUE(YES, NO);

    const Mengine::Char * BUILD_VERSION = Mengine::Helper::getBuildVersion();
    const Mengine::Char * BUILD_NUMBER_STRING = Mengine::Helper::getBuildNumberString();
    
#if defined(MENGINE_BUILD_PUBLISH_VALUE)
    const Mengine::Char * ENVIRONMENT = "production";
#else
    const Mengine::Char * ENVIRONMENT = "debug";
#endif
    
    [SentrySDK startWithConfigureOptions:^(SentryOptions *options) {
        options.dsn = AppleSentryPlugin_DSN;
        options.debug = AppleSentryPlugin_Debug;
        options.releaseName = @(BUILD_VERSION);
        options.dist = @(BUILD_NUMBER_STRING);
        options.environment = @(ENVIRONMENT);
        
        options.beforeSend = ^SentryEvent * _Nullable(SentryEvent * _Nonnull event) {
            
            return event;
        };
        
        options.beforeBreadcrumb = ^SentryBreadcrumb * _Nullable(SentryBreadcrumb * _Nonnull breadcrumb) {
            
            return breadcrumb;
        };
    }];
    
    return YES;
}

@end
