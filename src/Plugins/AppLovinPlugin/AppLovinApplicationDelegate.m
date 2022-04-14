#import "AppLovinApplicationDelegate.h"

#import <AppLovinSDK/AppLovinSDK.h>

@implementation AppLovinApplicationDelegate

#pragma mark -

+ (void)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
//    NSString *appodealkey = [[NSBundle mainBundle] localizedStringForKey:@"AppLovinSdkKey" value:nil table:nil];
    
    [ALSdk shared].settings.isVerboseLogging = YES;
    // Please make sure to set the mediation provider value to @"max" to ensure proper functionality
    [ALSdk shared].mediationProvider = @"max";
//    [ALSdk shared].userIdentifier = @"USER_ID";
    
    [[ALSdk shared] initializeSdkWithCompletionHandler:^(ALSdkConfiguration *configuration) {
        NSLog(@"APpLovin inited");
//        [[ALSdk shared] showMediationDebugger];
    }];
}

+ (void)applicationWillResignActive:(UIApplication *)application{}
+ (void)applicationDidEnterBackground:(UIApplication *)application{}
+ (void)applicationWillEnterForeground:(UIApplication *)application{}
+ (void)applicationDidBecomeActive:(UIApplication *)application{}
+ (void)applicationWillTerminate:(UIApplication *)application{}

+ (void)application:(UIApplication*)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData*)deviceToken{}
+ (void)application:(UIApplication*)application didReceiveLocalNotification:(UILocalNotification*)notification{}
+ (void)application:(UIApplication*)application didReceiveRemoteNotification:(NSDictionary*)userInfo{}
+ (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler{}

+ (BOOL)application:(UIApplication*)application openURL:(NSURL*)url sourceApplication:(NSString*)sourceApplication annotation:(id)annotation{
    return NO;
}

+ (BOOL)application:(UIApplication *)app openURL:(NSURL*)url options:(NSDictionary *)options{
    return NO;
}

+ (BOOL)application:(UIApplication*)application handleOpenURL:(NSURL *)url{
    return NO;
}
@end
