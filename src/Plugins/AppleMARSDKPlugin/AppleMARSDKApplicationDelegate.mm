#import "AppleMARSDKApplicationDelegate.h"

#import "MARSDKCore/MARSDKCore.h"

@implementation AppleMARSDKApplicationDelegate

#pragma mark -

+ (void)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    NSDictionary *sdkconfig = [[NSBundle mainBundle] objectForInfoDictionaryKey: @"MARSDK"];
    [[MARSDK sharedInstance] initWithParams:sdkconfig];
    [[MARSDK sharedInstance] application:application didFinishLaunchingWithOptions:launchOptions];
}

#pragma mark - UISceneSession lifecycle

+ (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    [[MARSDK sharedInstance] application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
}

+ (void)application:(UIApplication *)application didReceiveRemoteNotification:(nonnull NSDictionary *)userInfo {
    [[MARSDK sharedInstance] application:application didReceiveRemoteNotification: userInfo];
}

+ (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void(^)(UIBackgroundFetchResult)) completionHandler {
    [[MARSDK sharedInstance] application:application didReceiveRemoteNotification: userInfo fetchCompletionHandler:completionHandler];
}

+ (void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification {
    [[MARSDK sharedInstance] application:application didReceiveLocalNotification:notification];
}

+ (void)applicationWillResignActive:(UIApplication *)application {
    /* Sent when the application is about to move from active to inactive state.
     This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins
     the transition to the background state.
     */
    
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    [[MARSDK sharedInstance] applicationWillResignActive:application];
}

+ (void)applicationDidEnterBackground:(UIApplication *)application {
    /* Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    */
    
    // If your application supports background execution, this method is called in stead of applicationWillTerminate: when the user quits.
    [[MARSDK sharedInstance] applicationDidEnterBackground:application];
}

+ (void)applicationWillEnterForeground:(UIApplication *)application {
    [[MARSDK sharedInstance] applicationWillEnterForeground:application];
}

+ (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    
    [[MARSDK sharedInstance] applicationDidBecomeActive:application];
}

+ (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    [[MARSDK sharedInstance] applicationWillTerminate:application];
}

+ (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url handled:(BOOL *)handler {
    *handler = TRUE;
    
    return [[MARSDK sharedInstance] application:application handleOpenURL:url];
}

+ (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation handled:(BOOL *)handler {
    *handler = TRUE;
    
    return [[MARSDK sharedInstance] application:application openURL:url sourceApplication:sourceApplication annotation:annotation];
}

+ (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<NSString *,id> *)options handled:(BOOL *)handler {
    *handler = TRUE;
    
    return [[MARSDK sharedInstance] application:application openURL:url options:options];
}

@end
