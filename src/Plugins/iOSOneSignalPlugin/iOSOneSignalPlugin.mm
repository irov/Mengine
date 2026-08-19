#import "iOSOneSignalPlugin.h"

#import "Environment/Apple/AppleBundle.h"
#include "Environment/iOS/iOSDetail.h"

#import <OneSignalFramework/OneSignalFramework.h>

@implementation iOSOneSignalPlugin

#pragma mark - iOSPluginInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if ([AppleBundle hasPluginConfig:@"MengineiOSOneSignalPlugin"] == NO) {
        return YES;
    }

    NSString * MengineiOSOneSignalPlugin_AppId = [AppleBundle getPluginConfigString:@"MengineiOSOneSignalPlugin" withKey:@"AppId" withDefault:nil];

#ifdef MENGINE_DEBUG
    [OneSignal.Debug setLogLevel:ONE_S_LL_VERBOSE];
#endif

    if( MengineiOSOneSignalPlugin_AppId == nil )
    {
        return YES;
    }

    [OneSignal initialize:MengineiOSOneSignalPlugin_AppId withLaunchOptions:launchOptions];

    return YES;
}

@end
