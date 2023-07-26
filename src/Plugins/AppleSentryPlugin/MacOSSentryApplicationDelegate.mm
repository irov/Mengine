#import "MacOSSentryApplicationDelegate.h"

#import "Environment/Apple/AppleDetail.h"

#include "Kernel/BuildMode.h"

#import <Sentry/Sentry.h>

@implementation MacOSSentryApplicationDelegate

#pragma mark - MacOSProxyApplicationDelegateInterface

- (BOOL)application {
    NSString * AppleSentryPlugin_DSN = Mengine::Helper::AppleGetBundlePluginConfigString( @("MengineAppleSentryPlugin"), @("DSN"), nil );
    
    if( AppleSentryPlugin_DSN == nil )
    {
        return YES;
    }

    BOOL AppleSentryPlugin_Debug = Mengine::Helper::AppleGetBundlePluginConfigBoolean( @("MengineAppleSentryPlugin"), @("Debug"), NO );
    
    const Mengine::Char * BUILD_VERSION = Mengine::Helper::getBuildVersion();
    const Mengine::Char * BUILD_NUMBER_STRING = Mengine::Helper::getBuildNumberString();
    
#if defined(MENGINE_BUILD_PUBLISH_VALUE)
    const Mengine::Char * ENVIRONMENT = "publish";
#else
    const Mengine::Char * ENVIRONMENT = "dev";
#endif
    
    [SentrySDK startWithConfigureOptions:^(SentryOptions *options) {
        options.dsn = AppleSentryPlugin_DSN;
        options.debug = AppleSentryPlugin_Debug; // Enabled debug when first installing is always helpful
        options.releaseName = @(BUILD_VERSION);
        options.dist = @(BUILD_NUMBER_STRING);
        options.environment = @(ENVIRONMENT);
    }];
    
    return YES;
}

@end
