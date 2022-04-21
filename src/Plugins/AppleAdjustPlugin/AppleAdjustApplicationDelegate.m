#import "AppleAdjustApplicationDelegate.h"

#import "Adjust.h"

@implementation AppleAdjustApplicationDelegate

#pragma mark -

+ (void)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {}

+ (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {}

+ (void)application:(UIApplication *)application didReceiveRemoteNotification:(nonnull NSDictionary *)userInfo {}

+ (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void(^)(UIBackgroundFetchResult)) completionHandler {}

+ (void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification {
}

+ (void)applicationWillResignActive:(UIApplication *)application {}

+ (void)applicationDidEnterBackground:(UIApplication *)application {}

+ (void)applicationWillEnterForeground:(UIApplication *)application {}

+ (void)applicationDidBecomeActive:(UIApplication *)application {}

+ (void)applicationWillTerminate:(UIApplication *)application {}

+ (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url {
    [Adjust appWillOpenUrl:url];
    return YES;
}

+ (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation {
    [Adjust appWillOpenUrl:url];
    return YES;
}

+ (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<NSString *,id> *)options {
    [Adjust appWillOpenUrl:url];
    return YES;
}

@end
