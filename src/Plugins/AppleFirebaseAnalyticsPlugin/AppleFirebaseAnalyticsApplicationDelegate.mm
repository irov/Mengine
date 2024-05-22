#import "AppleFirebaseAnalyticsApplicationDelegate.h"

#include "Environment/Apple/AppleBundle.h"
#include "Environment/Apple/AppleLog.h"
#include "Environment/Apple/AppleSemaphoreService.h"

#include "Environment/iOS/iOSTransparencyConsentParam.h"

#include "Kernel/ThreadHelper.h"
#include "Kernel/ConstStringHelper.h"

#define PLUGIN_BUNDLE_NAME "MengineAppleFirebaseAnalyticsPlugin"

@implementation AppleFirebaseAnalyticsApplicationDelegate

#pragma mark - UIPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    //Empty
    
    return YES;
}

- (void)event:(NSString *)name args:(NSArray *)args {
    
}

@end
