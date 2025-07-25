#pragma once

#include "Config/Config.h"

#import "Environment/Apple/AppleEvent.h"

#import <UIKit/UIKit.h>

@protocol iOSPluginApplicationDelegateInterface <NSObject>

#pragma mark - UIApplicationDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions;

@optional
- (BOOL)beginLoop;
- (void)endLoop;

- (void)onEvent:(AppleEvent *)event args:(NSArray *)args;

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

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary *)options handled:(BOOL *)handler;

@end
