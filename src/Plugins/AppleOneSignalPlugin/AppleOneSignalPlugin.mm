#import "AppleOneSignalPlugin.h"

#include "Environment/iOS/iOSDetail.h"

#import <OneSignal/OneSignal.h>

@implementation AppleOneSignalPlugin

#pragma mark - iOSPluginInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if (Mengine::Helper::iOSHasBundlePluginConfig(@"MengineAppleOneSignalPlugin") == NO) {
        return YES;
    }
    
    NSString * MengineAppleOneSignalPlugin_AppId = Mengine::Helper::AppleGetBundlePluginConfigString(@"MengineAppleOneSignalPlugin", @"AppId", nil);
    
#ifdef MENGINE_DEBUG
    [OneSignal setLogLevel:ONE_S_LL_VERBOSE visualLevel:ONE_S_LL_NONE];
#endif
    
    if( MengineAppleOneSignalPlugin_AppId == nil )
    {
        return YES;
    }

    [OneSignal initWithLaunchOptions:launchOptions];
    [OneSignal setAppId:MengineAppleOneSignalPlugin_AppId];
    
    return YES;
}

@end
