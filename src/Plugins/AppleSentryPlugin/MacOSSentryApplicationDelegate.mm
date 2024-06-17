#import "MacOSSentryApplicationDelegate.h"

#import "Environment/Apple/AppleBundle.h"

#include "Kernel/BuildMode.h"

#import <Sentry/Sentry.h>

@implementation MacOSSentryApplicationDelegate

#pragma mark - MacOSProxyApplicationDelegateInterface

- (BOOL)application {
    NSString * MengineMacOSSentryPlugin_DSN = Mengine::Helper::AppleGetBundlePluginConfigString( @("MengineMacOSSentryPlugin"), @("DSN"), nil );
    
    if( MengineMacOSSentryPlugin_DSN == nil ) {
        return NO;
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
    
    return YES;
}

@end
