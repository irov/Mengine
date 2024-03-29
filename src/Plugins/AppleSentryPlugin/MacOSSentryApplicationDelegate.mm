#import "MacOSSentryApplicationDelegate.h"

#import "Environment/Apple/AppleDetail.h"

#include "Kernel/BuildMode.h"

#import <Sentry/Sentry.h>

@implementation MacOSSentryApplicationDelegate

#pragma mark - MacOSProxyApplicationDelegateInterface

- (BOOL)application {
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
    }];
    
    return YES;
}

@end
