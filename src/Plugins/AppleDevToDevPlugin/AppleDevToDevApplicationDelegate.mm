#import "AppleDevToDevApplicationDelegate.h"

#import "Environment/iOS/iOSDetail.h"

#import <DTDAnalytics/DTDAnalytics-Swift.h>

@implementation AppleDevToDevApplicationDelegate

#pragma mark - UIPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if (Mengine::Helper::iOSHasBundlePluginConfig(@"MengineAppleDevToDevPlugin") == NO) {
        return NO;
    }
    
    NSString * MengineAppleDevToDevPlugin_AppKey = Mengine::Helper::iOSGetBundlePluginConfigString(@"MengineAppleDevToDevPlugin", @"AppKey", nil);

    DTDAnalyticsConfiguration * config = [[DTDAnalyticsConfiguration alloc] init];

#ifdef MENGINE_DEBUG
    config.logLevel = DTDLogLevelDebug;
#else
    config.logLevel = DTDLogLevelError;
#endif

    [DTDAnalytics applicationKey:MengineAppleDevToDevPlugin_AppKey configuration:config];
    
    return YES;
}

@end
