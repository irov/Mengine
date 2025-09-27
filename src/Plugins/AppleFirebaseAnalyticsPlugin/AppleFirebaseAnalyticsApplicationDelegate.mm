#import "AppleFirebaseAnalyticsApplicationDelegate.h"

#import "Environment/Apple/AppleDetail.h"

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
    NSString * userId = [iOSApplication.sharedInstance getUserId];
    
    [FIRAnalytics setUserID:userId];
    
    NSString * installId = [iOSApplication.sharedInstance getInstallId];
    NSInteger installTimestamp = [iOSApplication.sharedInstance getInstallTimestamp];
    NSString * intstallVersion = [iOSApplication.sharedInstance getInstallVersion];
    NSInteger installRND = [iOSApplication.sharedInstance getInstallRND];
    NSInteger sessionIndex = [iOSApplication.sharedInstance getSessionIndex];
    NSInteger sessionTimestamp = [iOSApplication.sharedInstance getSessionTimestamp];
    
    [FIRAnalytics setUserPropertyString:MENGINE_PUBLISH_VALUE(@"true", @"false") forName:@"is_publish"];
    [FIRAnalytics setUserPropertyString:MENGINE_DEBUG_VALUE(@"true", @"false") forName:@"is_debug"];
    [FIRAnalytics setUserPropertyString:installId forName:@"install_id"];
    [FIRAnalytics setUserPropertyString:[NSString stringWithFormat:@"%ld", (long)installTimestamp] forName:@"install_timestamp"];
    [FIRAnalytics setUserPropertyString:intstallVersion forName:@"install_version"];
    [FIRAnalytics setUserPropertyString:[NSString stringWithFormat:@"%ld", (long)installRND] forName:@"install_rnd"];
    [FIRAnalytics setUserPropertyString:[NSString stringWithFormat:@"%ld", (long)sessionIndex] forName:@"session_index"];
    [FIRAnalytics setUserPropertyString:[NSString stringWithFormat:@"%ld", (long)sessionTimestamp] forName:@"session_timestamp"];
    
    NSInteger currentTimestamp = [AppleDetail getTimestamp];
    NSInteger lifeTime = currentTimestamp - installTimestamp;
    
    [FIRAnalytics setUserPropertyString:[NSString stringWithFormat:@"%ld", (long)lifeTime] forName:@"life_time"];
    
    return YES;
}

- (void)onUserId:(iOSUserParam *)user {
    [FIRAnalytics setUserID:user.USER_ID];
}

- (void)onRemoveUserData {
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
