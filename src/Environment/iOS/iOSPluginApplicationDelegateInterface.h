#include "Config/Config.h"

#include "Interface/LoggerRecordInterface.h"

#import "Environment/Apple/AppleEvent.h"

#import <UIKit/UIKit.h>

@protocol iOSPluginApplicationDelegateInterface <NSObject>

#pragma mark - UIApplicationDelegate Protocol

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions;

@optional
- (BOOL)beginLoop;
- (void)endLoop;

- (void)onEvent:(AppleEvent *)event args:(NSArray *)args;
- (void)onLog:(const Mengine::LoggerRecordInterfacePtr &)record;

#pragma mark - UISceneSession lifecycle

- (void)applicationWillResignActive:(UIApplication *)application;
- (void)applicationDidEnterBackground:(UIApplication *)application;
- (void)applicationWillEnterForeground:(UIApplication *)application;
- (void)applicationDidBecomeActive:(UIApplication *)application;
- (void)applicationWillTerminate:(UIApplication *)application;

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken;
- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler;

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary *)options handled:(BOOL *)handler;

@end
