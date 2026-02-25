#import "AppleAdjustPlugin.h"

#include "Environment/Apple/AppleBundle.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "AppleAdjustScriptEmbedding.h"
#endif

#define PLUGIN_BUNDLE_NAME @"MengineAppleAdjustPlugin"

@implementation AppleAdjustPlugin

+ (instancetype)sharedInstance {
    static AppleAdjustPlugin * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleAdjustPlugin class]];
    });
    return sharedInstance;
}

#pragma mark - iOSPluginInterface

- (void)onRunBegin {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::addScriptEmbedding<Mengine::AppleAdjustScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE );
#endif
}

- (void)onStopEnd {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::removeScriptEmbedding<Mengine::AppleAdjustScriptEmbedding>();
#endif
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if (Mengine::Helper::AppleHasBundlePluginConfig(PLUGIN_BUNDLE_NAME) == NO) {
        return NO;
    }
    
    NSString * MengineAppleAdjustPlugin_AppToken = Mengine::Helper::AppleGetBundlePluginConfigString(PLUGIN_BUNDLE_NAME, @"AppToken", nil);
    double MengineAppleAdjustPlugin_DelayStart = Mengine::Helper::AppleGetBundlePluginConfigDouble(PLUGIN_BUNDLE_NAME, @"DelayStart", 0.0);
    
#ifdef MENGINE_DEBUG
    NSString *environment = ADJEnvironmentSandbox;
#else
    NSString *environment = ADJEnvironmentProduction;
#endif
    
    ADJConfig *adjustConfig = [ADJConfig configWithAppToken:MengineAppleAdjustPlugin_AppToken
                                                environment:environment];
    
#ifdef MENGINE_DEBUG
    [adjustConfig setLogLevel:ADJLogLevelVerbose];
#endif
    
    [adjustConfig setDelayStart:MengineAppleAdjustPlugin_DelayStart];
    [adjustConfig setDelegate:self];
    
    [Adjust appDidLaunch:adjustConfig];

    IOS_LOGGER_MESSAGE(@"[Adjust] adid: %s", [[Adjust adid] UTF8String]);

    [Adjust requestTrackingAuthorizationWithCompletionHandler:^(NSUInteger status) {
        switch (status) {
            case 0:
                IOS_LOGGER_MESSAGE(@"[Adjust] ATTrackingManagerAuthorizationStatusNotDetermined");
                break;
            case 1:
                IOS_LOGGER_MESSAGE(@"[Adjust] ATTrackingManagerAuthorizationStatusRestricted");
                break;
            case 2:
                IOS_LOGGER_MESSAGE(@"[Adjust] ATTrackingManagerAuthorizationStatusDenied");
                break;
            case 3:
                IOS_LOGGER_MESSAGE(@"[Adjust] ATTrackingManagerAuthorizationStatusAuthorized");
                break;
            default:
                IOS_LOGGER_MESSAGE(@"[Adjust] ATTrackingManagerAuthorizationStatus unknown: %lu", (unsigned long)status);
                break;
        }
    }];
    
    return YES;
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    [Adjust setDeviceToken:deviceToken];
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation handled:(BOOL *)handler {
    *handler = YES;
    
    [Adjust appWillOpenUrl:url];
    
    return YES;
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<NSString *,id> *)options handled:(BOOL *)handler {
    *handler = YES;
    
    [Adjust appWillOpenUrl:url];
    
    return YES;
}

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url handled:(BOOL *)handler {
    *handler = YES;
    
    [Adjust appWillOpenUrl:url];
    
    return YES;
}

#pragma mark - AppleAdjustApi

- (void)eventTraking:(NSString *)token {
    if (token == nil) {
        IOS_LOGGER_MESSAGE(@"[Adjust] eventTraking token is null");
        return;
    }

    IOS_LOGGER_MESSAGE(@"[Adjust] eventTraking token: %s", token.UTF8String);

    ADJEvent * event = [ADJEvent eventWithEventToken:token];

    [Adjust trackEvent:event];
}

- (void)revenueTracking:(NSString *)token amount:(double)amount currency:(NSString *)currency {
    if (token == nil) {
        IOS_LOGGER_MESSAGE(@"[Adjust] revenueTracking token is null");
        return;
    }

    NSString * adjustCurrency = currency != nil ? currency : @"EUR";

    IOS_LOGGER_MESSAGE(@"[Adjust] revenueTracking token: %s amount: %lf currency: %s"
        , token.UTF8String
        , amount
        , adjustCurrency.UTF8String
    );

    ADJEvent * event = [ADJEvent eventWithEventToken:token];

    [event setRevenue:amount currency:adjustCurrency];

    [Adjust trackEvent:event];
}

#pragma mark - AdjustDelegate

- (void)adjustAttributionChanged:(nullable ADJAttribution *)attribution {
    //ToDo
}

- (void)adjustEventTrackingSucceeded:(nullable ADJEventSuccess *)eventSuccessResponseData {
    //ToDo
}

- (void)adjustEventTrackingFailed:(nullable ADJEventFailure *)eventFailureResponseData {
    //ToDo
}

- (void)adjustSessionTrackingSucceeded:(nullable ADJSessionSuccess *)sessionSuccessResponseData {
    //ToDo
}

- (void)adjustSessionTrackingFailed:(nullable ADJSessionFailure *)sessionFailureResponseData {
    //ToDo
}

- (BOOL)adjustDeeplinkResponse:(nullable NSURL *)deeplink {
    return NO;
}

- (void)adjustConversionValueUpdated:(nullable NSNumber *)conversionValue {
    //ToDo
}

@end
