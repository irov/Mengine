#import "AppleFirebaseCrashlyticsApplicationDelegate.h"

#include "Environment/iOS/iOSApplication.h"

#import <FirebaseCrashlytics/FirebaseCrashlytics.h>

#define PLUGIN_BUNDLE_NAME "MengineAppleFirebaseAnalyticsPlugin"

@implementation AppleFirebaseCrashlyticsApplicationDelegate

#pragma mark - UIPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
#ifdef MENGINE_DEBUG
    [[FIRCrashlytics crashlytics] setCrashlyticsCollectionEnabled:NO];
#else
    [[FIRCrashlytics crashlytics] setCrashlyticsCollectionEnabled:YES];
#endif
    
    NSString * sessionId = [iOSApplication.sharedInstance getSessionId];
    
    [[FIRCrashlytics crashlytics] setUserID:sessionId];
    
    return YES;
}

- (void)event:(NSString *)name args:(NSArray *)args {
    
}

@end
