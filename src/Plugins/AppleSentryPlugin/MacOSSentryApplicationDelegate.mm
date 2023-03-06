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
        return NO;
    }

    BOOL AppleSentryPlugin_Debug = Mengine::Helper::AppleGetBundlePluginConfigBoolean( @("MengineAppleSentryPlugin"), @("Debug"), NO );
    
    const Mengine::Char * BUILD_VERSION = Mengine::Helper::getBuildVersion();
    
    [SentrySDK startWithConfigureOptions:^(SentryOptions *options) {
        options.dsn = AppleSentryPlugin_DSN;
        options.debug = AppleSentryPlugin_Debug; // Enabled debug when first installing is always helpful
        options.releaseName = @(BUILD_VERSION);
        options.attachStacktrace = true;
        options.tracesSampleRate = @(1.0);
    }];
    
    return YES;
}

@end
