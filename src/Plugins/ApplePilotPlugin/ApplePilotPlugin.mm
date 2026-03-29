#import "ApplePilotPlugin.h"

#import "Environment/Apple/AppleBundle.h"
#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#include "Kernel/BuildMode.h"

@import PilotSDK;

#define PLUGIN_BUNDLE_NAME "MengineApplePilotPlugin"

@interface ApplePilotPlugin () <PilotObjCSessionDelegate, PilotObjCActionDelegate, PilotObjCLoggerDelegate>

@end

@implementation ApplePilotPlugin

+ (instancetype)sharedInstance {
    static ApplePilotPlugin *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[ApplePilotPlugin class]];
    });
    return sharedInstance;
}

#pragma mark - iOSPluginInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    (void)application;
    (void)launchOptions;

    NSString *apiUrl = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"ApiUrl" withDefault:nil];
    NSString *apiToken = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"ApiToken" withDefault:nil];

    if (apiToken == nil || apiToken.length == 0) {
        IOS_LOGGER_WARNING(@"Pilot API token not configured, plugin disabled");
        return YES;
    }

    if (apiUrl == nil || apiUrl.length == 0) {
        IOS_LOGGER_WARNING(@"Pilot API URL not configured, plugin disabled");
        return YES;
    }

    IOS_LOGGER_MESSAGE(@"Initializing Pilot SDK v%@", [PilotBridge sdkVersion]);

    NSString *installId = [[iOSApplication sharedInstance] getInstallId];
    NSInteger installTimestamp = [[iOSApplication sharedInstance] getInstallTimestamp];
    NSString *installVersion = [[iOSApplication sharedInstance] getInstallVersion];
    NSInteger installRND = [[iOSApplication sharedInstance] getInstallRND];
    NSInteger sessionIndex = [[iOSApplication sharedInstance] getSessionIndex];
    NSInteger sessionTimestamp = [[iOSApplication sharedInstance] getSessionTimestamp];

    PilotSessionAttributeBuilder *sessionAttrs = [[PilotSessionAttributeBuilder alloc] init];
    [sessionAttrs put:@"is_publish" value:@(MENGINE_BUILD_PUBLISH_VALUE(YES, NO))];
    [sessionAttrs put:@"is_debug" value:@(MENGINE_DEBUG_VALUE(YES, NO))];
    [sessionAttrs put:@"install_id" value:installId];
    [sessionAttrs put:@"install_timestamp" value:@(installTimestamp)];
    [sessionAttrs put:@"install_version" value:installVersion];
    [sessionAttrs put:@"install_rnd" value:@(installRND)];
    [sessionAttrs put:@"session_index" value:@(sessionIndex)];
    [sessionAttrs put:@"session_timestamp" value:@(sessionTimestamp)];
    [sessionAttrs putProvider:@"user_id" provider:^id _Nullable{
        return [[iOSApplication sharedInstance] getUserId];
    }];

    PilotLogConfigBuilder *logConfig = [[PilotLogConfigBuilder alloc] init];
    [logConfig setEnabled:YES];
    [logConfig setLogLevel:PilotObjCLogLevelInfo];

    PilotMetricConfigBuilder *metricConfig = [[PilotMetricConfigBuilder alloc] init];
    [metricConfig setEnabled:YES];
    [metricConfig setSampleIntervalMs:200];

    PilotConfigBuilder *config = [[PilotConfigBuilder alloc] initWithApiUrl:apiUrl apiToken:apiToken];
    [config setLoggerListener:self];
    [config setSessionListener:self];
    [config setActionListener:self];
    [config setSessionAttributes:sessionAttrs];
    [config setLogConfig:logConfig];
    [config setMetricConfig:metricConfig];

    [PilotBridge initialize:config];

    return YES;
}

- (void)applicationWillTerminate:(UIApplication *)application {
    (void)application;
    [PilotBridge shutdown];
}

#pragma mark - iOSPluginLoggerDelegateInterface

- (void)onLogger:(AppleLogRecordParam *)record {
    PilotObjCLogLevel level;

    switch (record.LOG_LEVEL) {
        case Mengine::LM_FATAL:
            level = PilotObjCLogLevelCritical;
            break;
        case Mengine::LM_ERROR:
            level = PilotObjCLogLevelError;
            break;
        case Mengine::LM_WARNING:
            level = PilotObjCLogLevelWarning;
            break;
        case Mengine::LM_MESSAGE_RELEASE:
        case Mengine::LM_MESSAGE:
        case Mengine::LM_INFO:
            level = PilotObjCLogLevelInfo;
            break;
        case Mengine::LM_DEBUG:
        case Mengine::LM_VERBOSE:
            level = PilotObjCLogLevelDebug;
            break;
        default:
            return;
    }

    [PilotBridge sendLogDetailed:level
                         message:record.LOG_DATA
                        category:record.LOG_CATEGORY
                          thread:record.LOG_THREAD];
}

#pragma mark - iOSPluginAnalyticDelegateInterface

- (void)onAnalyticEvent:(NSString *)event category:(iOSAnalyticsEventCategory)category params:(NSDictionary *)params {
    (void)category;
    [PilotBridge sendEvent:event metadata:params];
}

- (void)onAnalyticScreen:(NSString *)screen type:(NSString *)type {
    [PilotBridge changeScreenWithType:type name:screen];
}

- (void)onAnalyticFlush {
    // Pilot SDK handles flushing internally
}

#pragma mark - iOSPluginAdRevenueDelegateInterface

- (void)onAdRevenue:(iOSAdRevenueParam *)revenue {
    NSDictionary *metadata = @{
        @"ad_platform": revenue.REVENUE_PLATFORM,
        @"ad_source": revenue.REVENUE_SOURCE,
        @"ad_format": revenue.REVENUE_FORMAT,
        @"ad_unit_name": revenue.REVENUE_UNIT,
        @"placement": revenue.REVENUE_PLACEMENT ?: @"",
        @"currency": revenue.REVENUE_CURRENCY,
        @"value": revenue.REVENUE_VALUE
    };

    [PilotBridge sendRevenue:@"ad_impression" metadata:metadata];
}

#pragma mark - PilotObjCSessionDelegate

- (void)onPilotSessionConnecting {
    IOS_LOGGER_MESSAGE(@"Connecting to Pilot...");
}

- (void)onPilotSessionWaitingApproval:(NSString *)requestId {
    IOS_LOGGER_MESSAGE(@"Waiting for approval: %@", requestId);
}

- (void)onPilotSessionStarted:(NSString *)sessionToken {
    (void)sessionToken;
    IOS_LOGGER_MESSAGE(@"Pilot session started");
}

- (void)onPilotSessionClosed {
    IOS_LOGGER_MESSAGE(@"Pilot session closed");
}

- (void)onPilotSessionRejected {
    IOS_LOGGER_WARNING(@"Pilot connection rejected");
}

- (void)onPilotSessionAuthFailed {
    IOS_LOGGER_ERROR(@"Pilot authentication failed: invalid API token");
}

- (void)onPilotSessionError:(NSString *)message {
    IOS_LOGGER_ERROR(@"Pilot session error: %@", message);
}

#pragma mark - PilotObjCActionDelegate

- (void)onPilotActionReceived:(NSString *)actionId widgetId:(NSInteger)widgetId actionType:(NSString *)actionType {
    IOS_LOGGER_MESSAGE(@"Pilot action received: widget=%ld type=%@", (long)widgetId, actionType);
}

#pragma mark - PilotObjCLoggerDelegate

- (void)onPilotLoggerMessage:(PilotObjCLogLevel)level tag:(NSString *)tag message:(NSString *)message {
    switch (level) {
        case PilotObjCLogLevelDebug:
        case PilotObjCLogLevelInfo:
            IOS_LOGGER_MESSAGE(@"[%@] %@", tag, message);
            break;
        case PilotObjCLogLevelWarning:
            IOS_LOGGER_WARNING(@"[%@] %@", tag, message);
            break;
        case PilotObjCLogLevelError:
        case PilotObjCLogLevelCritical:
        case PilotObjCLogLevelException:
            IOS_LOGGER_ERROR(@"[%@] %@", tag, message);
            break;
    }
}

@end
