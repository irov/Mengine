#import "AppleMARSDKApplicationDelegate.h"

#import "Environment/Apple/AppleDetail.h"

#import "MARSDKCore/MARSDKCore.h"

@implementation AppleMARSDKApplicationDelegate

#pragma mark - UIProxyApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [[MARSDK sharedInstance] application:application didFinishLaunchingWithOptions:launchOptions];
    
    return YES;
}

#pragma mark - UISceneSession lifecycle

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

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    [[MARSDK sharedInstance] application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void(^)(UIBackgroundFetchResult)) completionHandler {
    [[MARSDK sharedInstance] application:application didReceiveRemoteNotification: userInfo fetchCompletionHandler:completionHandler];
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<NSString *,id> *)options handled:(BOOL *)handler {
    *handler = YES;
    
    return [[MARSDK sharedInstance] application:application openURL:url options:options];
}

@end
