#import "iOSOneSignalPlugin.h"

#include "Environment/iOS/iOSDetail.h"

#import <OneSignal/OneSignal.h>

@implementation iOSOneSignalPlugin

#pragma mark - iOSPluginInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if (Mengine::Helper::iOSHasBundlePluginConfig(@"MengineiOSOneSignalPlugin") == NO) {
        return YES;
    }

    NSString * MengineiOSOneSignalPlugin_AppId = Mengine::Helper::AppleGetBundlePluginConfigString(@"MengineiOSOneSignalPlugin", @"AppId", nil);

#ifdef MENGINE_DEBUG
    [OneSignal setLogLevel:ONE_S_LL_VERBOSE visualLevel:ONE_S_LL_NONE];
#endif

    if( MengineiOSOneSignalPlugin_AppId == nil )
    {
        return YES;
    }

    [OneSignal initWithLaunchOptions:launchOptions];
    [OneSignal setAppId:MengineiOSOneSignalPlugin_AppId];

    return YES;
}

@end
