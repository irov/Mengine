#import "iOSAdjustPlugin.h"

#import "Environment/Apple/AppleBundle.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Kernel/ScriptEmbeddingHelper.h"

#   include "iOSAdjustScriptEmbedding.h"
#endif

#import "ADJDeeplink.h"
#import "ADJEvent.h"
#import "ADJLogger.h"

#define PLUGIN_BUNDLE_NAME @"MengineiOSAdjustPlugin"

@implementation iOSAdjustPlugin

+ (instancetype)sharedInstance {
    static iOSAdjustPlugin * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[iOSAdjustPlugin class]];
    });
    return sharedInstance;
}

#pragma mark - iOSPluginInterface

- (void)onRunBegin {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::addScriptEmbedding<Mengine::iOSAdjustScriptEmbedding>( MENGINE_DOCUMENT_FUNCTION );
#endif
}

- (void)onStopEnd {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::removeScriptEmbedding<Mengine::iOSAdjustScriptEmbedding>();
#endif
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if ([AppleBundle hasPluginConfig:PLUGIN_BUNDLE_NAME] == NO) {
        return NO;
    }

    NSString * appToken = [AppleBundle getPluginConfigString:PLUGIN_BUNDLE_NAME withKey:@"AppToken" withDefault:nil];
    double delayStart = [AppleBundle getPluginConfigDouble:PLUGIN_BUNDLE_NAME withKey:@"DelayStart" withDefault:0.0];

#ifdef MENGINE_DEBUG
    NSString *environment = ADJEnvironmentSandbox;
#else
    NSString *environment = ADJEnvironmentProduction;
#endif

    ADJConfig * adjustConfig = [[ADJConfig alloc] initWithAppToken:appToken environment:environment];

#ifdef MENGINE_DEBUG
    [adjustConfig setLogLevel:ADJLogLevelVerbose];
#endif

    if( delayStart > 0.0 )
    {
        [adjustConfig enableFirstSessionDelay];
    }
    [adjustConfig setDelegate:self];

    [Adjust initSdk:adjustConfig];

    if( delayStart > 0.0 )
    {
        constexpr double maxDelayStart = 10.0;

        if( delayStart > maxDelayStart )
        {
            delayStart = maxDelayStart;
        }

        int64_t delayNanoseconds = static_cast<int64_t>(delayStart * NSEC_PER_SEC);
        dispatch_time_t delay = dispatch_time( DISPATCH_TIME_NOW, delayNanoseconds );
        dispatch_queue_t queue = dispatch_get_main_queue();

        dispatch_after( delay, queue, ^{
            [Adjust endFirstSessionDelay];
        } );
    }

    [Adjust adidWithCompletionHandler:^(NSString * adid) {
        const Mengine::Char * adidString = adid.UTF8String;

        IOS_LOGGER_MESSAGE(@"[Adjust] adid: %s", adidString);
    }];

    [Adjust requestAppTrackingAuthorizationWithCompletionHandler:^(NSUInteger status) {
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
    [Adjust setPushToken:deviceToken];
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<NSString *,id> *)options {

    ADJDeeplink * deeplink = [[ADJDeeplink alloc] initWithDeeplink:url];

    [Adjust processDeeplink:deeplink];

    return YES;
}

- (BOOL)application:(UIApplication *)application continueUserActivity:(NSUserActivity *)userActivity restorationHandler:(void (^)(NSArray<id<UIUserActivityRestoring>> * _Nullable restorableObjects))restorationHandler {
    MENGINE_UNUSED( application );
    MENGINE_UNUSED( restorationHandler );

    if ([userActivity.activityType isEqualToString:NSUserActivityTypeBrowsingWeb] == NO) {
        return NO;
    }

    NSURL * incomingUrl = userActivity.webpageURL;

    if (incomingUrl == nil) {
        return NO;
    }

    ADJDeeplink * deeplink = [[ADJDeeplink alloc] initWithDeeplink:incomingUrl];

    [Adjust processDeeplink:deeplink];

    return YES;
}

#pragma mark - iOSAdjustApi

- (void)eventTraking:(NSString *)token {
    if (token == nil) {
        IOS_LOGGER_MESSAGE(@"[Adjust] eventTraking token is null");
        return;
    }

    IOS_LOGGER_MESSAGE(@"[Adjust] eventTraking token: %s", token.UTF8String);

    ADJEvent * event = [[ADJEvent alloc] initWithEventToken:token];

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

    ADJEvent * event = [[ADJEvent alloc] initWithEventToken:token];

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
