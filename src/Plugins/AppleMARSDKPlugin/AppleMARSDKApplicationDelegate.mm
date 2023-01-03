#import "AppleMARSDKApplicationDelegate.h"

#import "Environment/iOS/iOSDetail.h"

#import "MARSDKCore/MARSDKCore.h"

@implementation AppleMARSDKApplicationDelegate

#pragma mark - UIKitProxyApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    NSDictionary * MARSDKConfig = Mengine::Helper::iOSGetBundlePluginConfig(@"MARSDK");
    [[MARSDK sharedInstance] initWithParams:MARSDKConfig];
    
    [[MARSDK sharedInstance] application:application didFinishLaunchingWithOptions:launchOptions];
    
    return YES;
}

#pragma mark - UISceneSession lifecycle

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    [[MARSDK sharedInstance] application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(nonnull NSDictionary *)userInfo {
    [[MARSDK sharedInstance] application:application didReceiveRemoteNotification: userInfo];
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void(^)(UIBackgroundFetchResult)) completionHandler {
    [[MARSDK sharedInstance] application:application didReceiveRemoteNotification: userInfo fetchCompletionHandler:completionHandler];
}

- (void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification {
    [[MARSDK sharedInstance] application:application didReceiveLocalNotification:notification];
}

- (void)applicationWillResignActive:(UIApplication *)application {
    [[MARSDK sharedInstance] applicationWillResignActive:application];
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    [[MARSDK sharedInstance] applicationDidEnterBackground:application];
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    [[MARSDK sharedInstance] applicationWillEnterForeground:application];
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    [[MARSDK sharedInstance] applicationDidBecomeActive:application];
}

- (void)applicationWillTerminate:(UIApplication *)application {
    [[MARSDK sharedInstance] applicationWillTerminate:application];
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation handled:(BOOL *)handler {
    *handler = YES;
    
    return [[MARSDK sharedInstance] application:application openURL:url sourceApplication:sourceApplication annotation:annotation];
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<NSString *,id> *)options handled:(BOOL *)handler {
    *handler = YES;
    
    return [[MARSDK sharedInstance] application:application openURL:url options:options];
}

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url handled:(BOOL *)handler {
    *handler = YES;
    
    return [[MARSDK sharedInstance] application:application handleOpenURL:url];
}

@end
