#import "AppleFirebaseAnalyticsApplicationDelegate.h"

#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSRevenueParam.h"

#import <FirebaseAnalytics/FirebaseAnalytics.h>

#define PLUGIN_BUNDLE_NAME "MengineAppleFirebaseAnalyticsPlugin"

@implementation AppleFirebaseAnalyticsApplicationDelegate

#pragma mark - UIPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    NSString * sessionId = [iOSApplication.sharedInstance getSessionId];
    
    [FIRAnalytics setUserID:sessionId];
    
    return YES;
}

- (void)onEvent:(AppleEvent *)event args:(NSArray *)args {
    if (event == AppleEvent.EVENT_SESSION_ID) {
        NSString * sessionId = args[0];
        
        [FIRAnalytics setUserID:sessionId];
    } else if (event == AppleEvent.EVENT_REVENUE) {
        iOSRevenueParam * revenue = args[0];
        
        [FIRAnalytics logEventWithName:kFIREventAdImpression
                            parameters:@{
                                kFIRParameterAdPlatform:revenue.REVENUE_PLATFORM,
                                kFIRParameterAdSource:revenue.REVENUE_SOURCE,
                                kFIRParameterAdFormat:revenue.REVENUE_FORMAT,
                                kFIRParameterAdUnitName:revenue.REVENUE_UNIT,
                                kFIRParameterCurrency:revenue.REVENUE_CURRENCY,
                                kFIRParameterValue:revenue.REVENUE_VALUE}];
    }
}

@end
