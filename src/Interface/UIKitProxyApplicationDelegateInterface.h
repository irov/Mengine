#import <UIKit/UIKit.h>

@protocol UIKitProxyApplicationDelegateInterface

+ (void)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions;
+ (void)applicationWillResignActive:(UIApplication *)application;
+ (void)applicationDidEnterBackground:(UIApplication *)application;
+ (void)applicationWillEnterForeground:(UIApplication *)application;
+ (void)applicationDidBecomeActive:(UIApplication *)application;
+ (void)applicationWillTerminate:(UIApplication *)application;

+ (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken;
+ (void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification;
+ (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo;
+ (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler;

+ (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation handled:(BOOL *)handler;
+ (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary *)options handled:(BOOL *)handler;
+ (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url handled:(BOOL *)handler;

@end
