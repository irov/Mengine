#import "AppleFirebaseAnalyticsApplicationDelegate.h"

#import "Environment/iOS/iOSApplication.h"

#import <FirebaseAnalytics/FirebaseAnalytics.h>

#define PLUGIN_BUNDLE_NAME "MengineAppleFirebaseAnalyticsPlugin"

@implementation AppleFirebaseAnalyticsApplicationDelegate

#pragma mark - UIPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    NSString * sessionId = [iOSApplication.sharedInstance getSessionId];
    
    [FIRAnalytics setUserID:sessionId];
    
    return YES;
}

- (void)onEvent:(MengineEvent *)event args:(NSArray *)args {
    if (event == MengineEvent.EVENT_SESSION_ID) {
        NSString * sessionId = args[0];
        
        [FIRAnalytics setUserID:sessionId];
    }
}

@end
