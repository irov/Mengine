#import "UIKitProxyApplicationDelegateBase.h"

@implementation UIKitProxyApplicationDelegateBase

#pragma mark - UIKitProxyApplicationDelegateInterface

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    //Empty
    
    return YES;
}

#pragma mark - UISceneSession lifecycle

- (void)applicationWillResignActive:(UIApplication *)application {
    //Empty
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    //Empty
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    //Empty
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    //Empty
}

- (void)applicationWillTerminate:(UIApplication *)application {
    //Empty
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    //Empty
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo {
    //Empty
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler {
    //Empty
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation handled:(BOOL *)handler {
    //Empty
    
    return NO;
}
    
- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary *)options handled:(BOOL *)handler {
    //Empty
    
    return NO;
}

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url handled:(BOOL *)handler {
    //Empty
    
    return NO;
}

@end
