#pragma once

#include "Config/Config.h"

#import <UIKit/UIKit.h>

@protocol iOSPluginInterface <NSObject>

#pragma mark - UIApplicationDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions;

@optional
- (void)onBootstrapBegin:(NSArray<NSString *> *)arguments;
- (void)onBootstrapEnd;
- (void)onRunBegin;
- (void)onRunEnd;
- (void)onLoopBegin;
- (void)onLoopEnd;
- (void)onStopBegin;
- (void)onStopEnd;
- (void)onFinalize;

#pragma mark - UISceneSession

@optional
- (BOOL)application:(UIApplication *)application didPostLaunchingWithOptions:(NSDictionary *)launchOptions;

- (void)applicationWillResignActive:(UIApplication *)application;
- (void)applicationDidEnterBackground:(UIApplication *)application;
- (void)applicationWillEnterForeground:(UIApplication *)application;
- (void)applicationDidBecomeActive:(UIApplication *)application;
- (void)applicationWillTerminate:(UIApplication *)application;

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken;
- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler;

// opened is YES only for the default action on a remote notification.
- (BOOL)handleRemoteNotification:(NSDictionary *)userInfo opened:(BOOL)opened;

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary *)options;
- (BOOL)application:(UIApplication *)application continueUserActivity:(NSUserActivity *)userActivity restorationHandler:(void (^)(NSArray<id<UIUserActivityRestoring>> * restorableObjects))restorationHandler;

@end
