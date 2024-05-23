#import "AppleFirebaseAnalyticsApplicationDelegate.h"

#include "Environment/iOS/iOSApplication.h"

#import <FirebaseAnalytics/FirebaseAnalytics.h>

#define PLUGIN_BUNDLE_NAME "MengineAppleFirebaseAnalyticsPlugin"

@implementation AppleFirebaseAnalyticsApplicationDelegate

#pragma mark - UIPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    NSString * sessionId = [iOSApplication.sharedInstance getSessionId];
    
    [FIRAnalytics setUserID:sessionId];
    
    return YES;
}

- (void)event:(NSString *)name args:(NSArray *)args {
    
}

@end
