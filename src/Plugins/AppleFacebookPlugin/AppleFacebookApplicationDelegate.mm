#import "AppleFacebookApplicationDelegate.h"

#import <AuthenticationServices/AuthenticationServices.h>
#import <SafariServices/SafariServices.h>
#import <FBSDKCoreKit/FBSDKCoreKit-Swift.h>

@implementation AppleFacebookApplicationDelegate

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [FBSDKApplicationDelegate.sharedInstance application:application didFinishLaunchingWithOptions:launchOptions];

    return YES;
}

#pragma mark - UISceneSession lifecycle

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<NSString *, id> *)options handled:(BOOL *)handler {
    *handler = YES;
    
    return [FBSDKApplicationDelegate.sharedInstance application:application openURL:url options:options];
}

@end
