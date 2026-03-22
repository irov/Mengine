#pragma once

#import "Environment/iOS/iOSUIMainApplicationDelegateInterface.h"

#import <UIKit/UIKit.h>

@interface iOSUIApplicationDelegate : NSObject<UIApplicationDelegate, iOSUIMainApplicationDelegateInterface>

- (void)handleApplicationDidBecomeActive:(UIApplication *)application;
- (void)handleApplicationWillEnterForeground:(UIApplication *)application;
- (void)handleApplicationDidEnterBackground:(UIApplication *)application;
- (void)handleApplicationWillResignActive:(UIApplication *)application;
- (void)handleApplicationWillTerminate:(UIApplication *)application;

@end
