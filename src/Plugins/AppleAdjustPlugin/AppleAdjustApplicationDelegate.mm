#import "AppleAdjustApplicationDelegate.h"

#include "Environment/iOS/iOSDetail.h"

@implementation AppleAdjustApplicationDelegate

#pragma mark - UIPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if (Mengine::Helper::iOSHasBundlePluginConfig(@"MengineAppleAdjustPlugin") == NO) {
        return NO;
    }
    
    NSString * MengineAppleAdjustPlugin_AppToken = Mengine::Helper::iOSGetBundlePluginConfigString(@"MengineAppleAdjustPlugin", @"AppToken", nil);
    double MengineAppleAdjustPlugin_DelayStart = Mengine::Helper::iOSGetBundlePluginConfigDouble(@"MengineAppleAdjustPlugin", @"DelayStart", 0.0);
    
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
