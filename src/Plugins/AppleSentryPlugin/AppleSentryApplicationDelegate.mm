#import "AppleSentryApplicationDelegate.h"

#import "Environment/Apple/AppleDetail.h"

#include "Kernel/BuildMode.h"

#import "Sentry/Sentry.h"

@implementation AppleSentryApplicationDelegate

#pragma mark - UIKitProxyApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {   
    NSString * AppleSentryPlugin_DSN = Mengine::Helper::AppleGetBundlePluginConfigString( @("MengineAppleSentryPlugin"), @("DSN"), nil );
    
    if( AppleSentryPlugin_DSN == nil )
    {
        return NO;
    }

    const Mengine::Char * BUILD_VERSION = Mengine::Helper::getBuildVersion();
    
    [SentrySDK startWithConfigureOptions:^(SentryOptions *options) {
        options.dsn = AppleSentryPlugin_DSN;
        options.debug = MENGINE_DEBUG_VALUE(YES, NO); // Enabled debug when first installing is always helpful
        options.environment = MENGINE_BUILD_PUBLISH_VALUE(@"production", @"debug");
        options.releaseName = @(BUILD_VERSION);
    }];
    
    return YES;
}

@end
