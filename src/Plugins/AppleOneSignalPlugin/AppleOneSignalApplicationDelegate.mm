#import "AppleOneSignalApplicationDelegate.h"

#include "Environment/iOS/iOSDetail.h"

#import <OneSignal/OneSignal.h>

@implementation AppleOneSignalApplicationDelegate

#pragma mark - UIKitProxyApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if (Mengine::Helper::iOSHasBundlePluginConfig(@"MengineAppleOneSignalPlugin") == NO) {
        return NO;
    }
    
    NSString * MengineAppleOneSignalPlugin_AppId = Mengine::Helper::iOSGetBundlePluginConfigString(@"MengineAppleOneSignalPlugin", @"AppId", nil);
    
#ifdef MENGINE_DEBUG
    [OneSignal setLogLevel:ONE_S_LL_VERBOSE visualLevel:ONE_S_LL_NONE];
#endif
    
    [OneSignal initWithLaunchOptions:launchOptions];
    [OneSignal setAppId:MengineAppleOneSignalPlugin_AppId];
    
    return YES;
}

@end
