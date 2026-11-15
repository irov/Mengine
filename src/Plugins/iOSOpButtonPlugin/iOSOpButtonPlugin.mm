#import "iOSOpButtonPlugin.h"

#if defined(MENGINE_PLUGIN_IOS_USERNOTIFICATIONCENTER)
#   import "Plugins/iOSUserNotificationCenterPlugin/iOSUserNotificationCenterPlugin.h"
#endif

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "iOSOpButtonScriptEmbedding.h"
#endif

#import "Environment/Apple/AppleBundle.h"
#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleSemaphoreService.h"

#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#include "Kernel/BuildMode.h"
#include "Kernel/ScriptEmbeddingHelper.h"

@import OpButtonSDK;

#define PLUGIN_BUNDLE_NAME "MengineiOSOpButtonPlugin"

static int64_t const MENGINE_OPBUTTON_METRIC_SAMPLE_INTERVAL_MILLISECONDS = 200;

@interface iOSOpButtonPlugin () <OpButtonObjCSessionDelegate, OpButtonObjCActionDelegate, OpButtonObjCLoggerDelegate>
@end

@implementation iOSOpButtonPlugin
{
    NSMutableDictionary<NSString *, OpButtonCommandTab *> * m_commandTabs;
    OpButtonCommandTab * m_notificationsTab;
}

+ (instancetype)sharedInstance {
    static iOSOpButtonPlugin * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[iOSOpButtonPlugin class]];
    });
    return sharedInstance;
}

- (BOOL)addCommandButton:(NSString *)tabName identifier:(NSString *)identifier title:(NSString *)title handler:(void (^)(void))handler {
    if ([OpButtonBridge isInitialized] == NO) {
        return NO;
    }

    OpButtonCommandTab * tab = m_commandTabs[tabName];

    if (tab == nil) {
        tab = [OpButtonBridge createCommandTab:tabName title:tabName];
        m_commandTabs[tabName] = tab;
    }

    [tab addButton:identifier title:title onClick:handler];

    return YES;
}

- (void)removeCommandTab:(NSString *)tabName {
    OpButtonCommandTab * tab = m_commandTabs[tabName];
    [m_commandTabs removeObjectForKey:tabName];
    [tab close];
}

#pragma mark - iOSPluginInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    (void)application;
    (void)launchOptions;

    NSString * apiUrl = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"ApiUrl" withDefault:nil];
    NSString * apiToken = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"ApiToken" withDefault:nil];

    if (apiToken.length == 0) {
        IOS_LOGGER_WARNING(@"OpButton API token not configured, plugin disabled");
        return YES;
    }

    if (apiUrl.length == 0) {
        IOS_LOGGER_WARNING(@"OpButton API URL not configured, plugin disabled");
        return YES;
    }

    IOS_LOGGER_MESSAGE(@"Initializing OpButton SDK v%@", [OpButtonBridge sdkVersion]);

    NSString * installId = [[iOSApplication sharedInstance] getInstallId];
    NSInteger installTimestamp = [[iOSApplication sharedInstance] getInstallTimestamp];
    NSString * installVersion = [[iOSApplication sharedInstance] getInstallVersion];
    NSInteger installRND = [[iOSApplication sharedInstance] getInstallRND];
    NSInteger sessionIndex = [[iOSApplication sharedInstance] getSessionIndex];
    NSInteger sessionTimestamp = [[iOSApplication sharedInstance] getSessionTimestamp];

    OpButtonSessionAttributeBuilder * sessionAttrs = [[OpButtonSessionAttributeBuilder alloc] init];
    NSNumber * publishAttribute = @(MENGINE_BUILD_PUBLISH_VALUE(YES, NO));
    [sessionAttrs put:@"is_publish" value:publishAttribute];
    NSNumber * debugAttribute = @(MENGINE_DEBUG_VALUE(YES, NO));
    [sessionAttrs put:@"is_debug" value:debugAttribute];
    [sessionAttrs put:@"install_id" value:installId];
    NSNumber * installTimestampAttribute = @(installTimestamp);
    [sessionAttrs put:@"install_timestamp" value:installTimestampAttribute];
    [sessionAttrs put:@"install_version" value:installVersion];
    NSNumber * installRNDAttribute = @(installRND);
    [sessionAttrs put:@"install_rnd" value:installRNDAttribute];
    NSNumber * sessionIndexAttribute = @(sessionIndex);
    [sessionAttrs put:@"session_index" value:sessionIndexAttribute];
    NSNumber * sessionTimestampAttribute = @(sessionTimestamp);
    [sessionAttrs put:@"session_timestamp" value:sessionTimestampAttribute];
    [sessionAttrs putProvider:@"user_id" provider:^id _Nullable {
        NSString * userId = [[iOSApplication sharedInstance] getUserId];

        return userId;
    }];

    OpButtonLogConfigBuilder * logConfig = [[OpButtonLogConfigBuilder alloc] init];
    [logConfig setEnabled:YES];
    [logConfig setLogLevel:OpButtonObjCLogLevelInfo];

    OpButtonMetricConfigBuilder * metricConfig = [[OpButtonMetricConfigBuilder alloc] init];
    [metricConfig setEnabled:YES];
    [metricConfig setSampleIntervalMs:MENGINE_OPBUTTON_METRIC_SAMPLE_INTERVAL_MILLISECONDS];

    OpButtonConfigBuilder * config = [[OpButtonConfigBuilder alloc] initWithApiUrl:apiUrl apiToken:apiToken];
    [config setLoggerListener:self];
    [config setSessionListener:self];
    [config setActionListener:self];
    [config setSessionAttributes:sessionAttrs];
    [config setLogConfig:logConfig];
    [config setMetricConfig:metricConfig];

    [OpButtonBridge initialize:config];
    m_commandTabs = [NSMutableDictionary dictionary];

    return YES;
}

- (void)onRunBegin {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::addScriptEmbedding<Mengine::iOSOpButtonScriptEmbedding>( MENGINE_DOCUMENT_FUNCTION );
#endif

    BOOL initialized = [OpButtonBridge isInitialized];

#if defined(MENGINE_PLUGIN_IOS_USERNOTIFICATIONCENTER)
    if (initialized == YES) {
        NSTimeInterval interval = [AppleBundle getPluginConfigDouble:@PLUGIN_BUNDLE_NAME withKey:@"NotificationIntervalSeconds" withDefault:0.0];

        if (interval > 0.0) {
            m_notificationsTab = [OpButtonBridge createCommandTab:@"Notifications" title:@"Notifications"];
            NSString * title = [NSString stringWithFormat:@"Queue notifications every %g seconds", interval];
            [m_notificationsTab addButton:@"notification_queue" title:title onClick:^{
                [[iOSUserNotificationCenterPlugin sharedInstance] queueNotificationsWithInterval:interval];
            }];
        }
    }
#endif

    NSNumber * successful = @(initialized);
    [AppleSemaphoreService.sharedInstance activateSemaphore:@"OpButtonReady" withValue:successful];
}

- (void)onStopEnd {
    [AppleSemaphoreService.sharedInstance deactivateSemaphore:@"OpButtonReady"];

    NSArray<NSString *> * tabs = [m_commandTabs allKeys];

    for (NSString * tab in tabs) {
        [self removeCommandTab:tab];
    }

    m_commandTabs = nil;

    [m_notificationsTab close];
    m_notificationsTab = nil;

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::removeScriptEmbedding<Mengine::iOSOpButtonScriptEmbedding>();
#endif
}

- (void)applicationWillTerminate:(UIApplication *)application {
    (void)application;
    [OpButtonBridge shutdown];
}

#pragma mark - iOSPluginLoggerDelegateInterface

- (void)onLogger:(AppleLogRecordParam *)record {
    if ([record.LOG_CATEGORY isEqualToString:@(MENGINE_CODE_LIBRARY)] == YES) {
        return;
    }

    OpButtonObjCLogLevel level;

    switch (record.LOG_LEVEL) {
        case Mengine::LM_FATAL:
            level = OpButtonObjCLogLevelCritical;
            break;
        case Mengine::LM_ERROR:
            level = OpButtonObjCLogLevelError;
            break;
        case Mengine::LM_WARNING:
            level = OpButtonObjCLogLevelWarning;
            break;
        case Mengine::LM_MESSAGE_RELEASE:
        case Mengine::LM_MESSAGE:
        case Mengine::LM_INFO:
            level = OpButtonObjCLogLevelInfo;
            break;
        case Mengine::LM_DEBUG:
        case Mengine::LM_VERBOSE:
            level = OpButtonObjCLogLevelDebug;
            break;
        default:
            return;
    }

    [OpButtonBridge sendLogDetailed:level
                         message:record.LOG_DATA
                        category:record.LOG_CATEGORY
                          thread:record.LOG_THREAD];
}

#pragma mark - iOSPluginAnalyticDelegateInterface

- (void)onAnalyticEvent:(NSString *)event category:(iOSAnalyticsEventCategory)category params:(NSDictionary *)params {
    (void)category;
    [OpButtonBridge sendEvent:event metadata:params];
}

- (void)onAnalyticScreen:(NSString *)screen type:(NSString *)type {
    [OpButtonBridge changeScreenWithType:type name:screen];
}

- (void)onAnalyticFlush {
    //Empty
}

#pragma mark - iOSPluginAdRevenueDelegateInterface

- (void)onAdRevenue:(iOSAdRevenueParam *)revenue {
    NSDictionary * metadata = @{
        @"ad_platform": revenue.REVENUE_PLATFORM,
        @"ad_source": revenue.REVENUE_SOURCE,
        @"ad_format": revenue.REVENUE_FORMAT,
        @"ad_unit_name": revenue.REVENUE_UNIT,
        @"placement": revenue.REVENUE_PLACEMENT ?: @"",
        @"currency": revenue.REVENUE_CURRENCY,
        @"value": revenue.REVENUE_VALUE
    };

    [OpButtonBridge sendRevenue:@"ad_impression" metadata:metadata];
}

#pragma mark - OpButtonObjCSessionDelegate

- (void)onOpButtonSessionConnecting {
    IOS_LOGGER_MESSAGE(@"Connecting to OpButton...");
}

- (void)onOpButtonSessionWaitingApproval:(NSString *)requestId {
    IOS_LOGGER_MESSAGE(@"Waiting for approval: %@", requestId);
}

- (void)onOpButtonSessionStarted:(NSString *)sessionToken {
    (void)sessionToken;
    IOS_LOGGER_MESSAGE(@"OpButton session started");
}

- (void)onOpButtonSessionClosed {
    IOS_LOGGER_MESSAGE(@"OpButton session closed");
}

- (void)onOpButtonSessionRejected {
    IOS_LOGGER_WARNING(@"OpButton connection rejected");
}

- (void)onOpButtonSessionAuthFailed {
    IOS_LOGGER_ERROR(@"OpButton authentication failed: invalid API token");
}

- (void)onOpButtonSessionError:(NSString *)message {
    IOS_LOGGER_ERROR(@"OpButton session error: %@", message);
}

#pragma mark - OpButtonObjCActionDelegate

- (void)onOpButtonActionReceived:(NSString *)actionId widgetId:(NSInteger)widgetId actionType:(NSString *)actionType {
    IOS_LOGGER_MESSAGE(@"OpButton action received: widget=%ld type=%@", (long)widgetId, actionType);
}

#pragma mark - OpButtonObjCLoggerDelegate

- (void)onOpButtonLoggerMessage:(OpButtonObjCLogLevel)level tag:(NSString *)tag message:(NSString *)message {
    switch (level) {
        case OpButtonObjCLogLevelDebug:
        case OpButtonObjCLogLevelInfo:
            IOS_LOGGER_MESSAGE(@"[%@] %@", tag, message);
            break;
        case OpButtonObjCLogLevelWarning:
            IOS_LOGGER_WARNING(@"[%@] %@", tag, message);
            break;
        case OpButtonObjCLogLevelError:
        case OpButtonObjCLogLevelCritical:
        case OpButtonObjCLogLevelException:
            IOS_LOGGER_ERROR(@"[%@] %@", tag, message);
            break;
    }
}

@end
