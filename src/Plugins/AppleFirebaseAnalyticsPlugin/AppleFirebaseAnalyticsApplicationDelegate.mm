#import "AppleFirebaseAnalyticsApplicationDelegate.h"

#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSAdRevenueParam.h"

#import <FirebaseAnalytics/FirebaseAnalytics.h>

#define PLUGIN_BUNDLE_NAME "MengineAppleFirebaseAnalyticsPlugin"

@implementation AppleFirebaseAnalyticsApplicationDelegate

+ (instancetype)sharedInstance {
    static AppleFirebaseAnalyticsApplicationDelegate *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleFirebaseAnalyticsApplicationDelegate class]];
    });
    return sharedInstance;
}

- (void)sendEvent:(NSString *)eventName parameters:(NSDictionary<NSString *, id> *)parameters {
    [FIRAnalytics logEventWithName:eventName parameters:parameters];
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    NSString * sessionId = [iOSApplication.sharedInstance getSessionId];
    
    [FIRAnalytics setUserID:sessionId];
    
    return YES;
}

- (void)onSessionId:(iOSSessionIdParam *)session {
    [FIRAnalytics setUserID:session.SESSION_ID];
}

- (void)onRemoveSessionData {
    [FIRAnalytics resetAnalyticsData];
    [FIRAnalytics setUserID:nil];
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
