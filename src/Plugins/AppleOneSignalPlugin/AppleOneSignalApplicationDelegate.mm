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
    
    // Remove this method to stop OneSignal Debugging  
#ifdef MENGINE_DEBUG
    [OneSignal setLogLevel:ONE_S_LL_VERBOSE visualLevel:ONE_S_LL_NONE];
#endif
      
    // OneSignal initialization
    [OneSignal initWithLaunchOptions:launchOptions];
    [OneSignal setAppId:MengineAppleOneSignalPlugin_AppId];

    // promptForPushNotifications will show the native iOS notification permission prompt.
    // We recommend removing the following code and instead using an In-App Message to prompt for notification permission (See step 8)
    [OneSignal promptForPushNotificationsWithUserResponse:^(BOOL accepted) {
        NSLog(@"Mengine user accepted notifications: %d", accepted);
    }];
    
    return YES;
}

@end
