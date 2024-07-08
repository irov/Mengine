#import "AppleFirebaseAnalyticsApplicationDelegate.h"

#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSAdRevenueParam.h"

#import <FirebaseAnalytics/FirebaseAnalytics.h>

#define PLUGIN_BUNDLE_NAME "MengineAppleFirebaseAnalyticsPlugin"

@implementation AppleFirebaseAnalyticsApplicationDelegate

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    NSString * sessionId = [iOSApplication.sharedInstance getSessionId];
    
    [FIRAnalytics setUserID:sessionId];
    
    return YES;
}

- (void)onEvent:(AppleEvent *)event args:(NSArray *)args {
    if (event == AppleEvent.EVENT_SESSION_ID) {
        NSString * sessionId = args[0];
        
        [FIRAnalytics setUserID:sessionId];
    }
}

#pragma mark - iOSPluginAdRevenueDelegateInterface

- (void)onAdRevenue:(iOSAdRevenueParam *)revenue {
    [FIRAnalytics logEventWithName:kFIREventAdImpression
                        parameters:@{
                            kFIRParameterAdPlatform:revenue.REVENUE_PLATFORM,
                            kFIRParameterAdSource:revenue.REVENUE_SOURCE,
                            kFIRParameterAdFormat:revenue.REVENUE_FORMAT,
                            kFIRParameterAdUnitName:revenue.REVENUE_UNIT,
                            kFIRParameterCurrency:revenue.REVENUE_CURRENCY,
                            kFIRParameterValue:revenue.REVENUE_VALUE}];
}

#pragma mark iOSPluginTransparencyConsentDelegateInterface

- (void)onTransparencyConsent:(iOSTransparencyConsentParam *)consent {
    BOOL AD_STORAGE = [consent getConsentAdStorage];
    BOOL ANALYTICS_STORAGE = [consent getConsentAnalyticsStorage];
    BOOL AD_PERSONALIZATION = [consent getConsentAdPersonalization];
    BOOL AD_USER_DATA = [consent getConsentAdUserData];
    
    [FIRAnalytics setConsent:@{
        FIRConsentTypeAdStorage : AD_STORAGE ? FIRConsentStatusGranted : FIRConsentStatusDenied,
        FIRConsentTypeAnalyticsStorage : ANALYTICS_STORAGE ? FIRConsentStatusGranted : FIRConsentStatusDenied,
        FIRConsentTypeAdPersonalization : AD_PERSONALIZATION ? FIRConsentStatusGranted : FIRConsentStatusDenied,
        FIRConsentTypeAdUserData : AD_USER_DATA ? FIRConsentStatusGranted : FIRConsentStatusDenied,
    }];
}

@end
