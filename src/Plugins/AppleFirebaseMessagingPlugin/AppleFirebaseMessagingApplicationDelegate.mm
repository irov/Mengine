#import "AppleFirebaseMessagingApplicationDelegate.h"

#import <Firebase/Firebase.h>

@implementation AppleFirebaseMessagingApplicationDelegate

#pragma mark - UIKitProxyApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [FBSDKApplicationDelegate.sharedInstance application:application didFinishLaunchingWithOptions:launchOptions];

    return YES;
}

#pragma mark - UISceneSession lifecycle

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation handled:(BOOL *)handler {
    *handler = YES;
    
    return [FBSDKApplicationDelegate.sharedInstance application:application openURL:url sourceApplication:sourceApplication annotation:annotation];
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<NSString *,id> *)options handled:(BOOL *)handler {
    *handler = YES;
    
    return [FBSDKApplicationDelegate.sharedInstance application:application openURL:url options:options];
}

@end
