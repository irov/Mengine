#import "AppleDatadogApplicationDelegate.h"

#import "Environment/Apple/AppleBundle.h"

#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#define PLUGIN_BUNDLE_NAME "MengineAppleDatadogPlugin"

@implementation AppleDatadogApplicationDelegate

+ (instancetype) sharedInstance {
    static AppleDatadogApplicationDelegate *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleDatadogApplicationDelegate class]];
    });
    return sharedInstance;
}

- (instancetype)init {
    self = [super init];
    
    if (self != nil) {
        self.m_enableDebugMessage = MENGINE_RELEASE_VALUE(NO, YES);
        self.m_enableInfoMessage = NO;
        
        self.m_logger = nil;
    }

    return self;
}

- (DDTrackingConsent *)getTrackingConsent:(iOSTransparencyConsentParam *)consent {
    if ([iOSTransparencyConsentParam getConsentFlowUserGeography] == iOSConsentFlowUserGeographyOther) {
        return [DDTrackingConsent granted];
    }
    
    if (consent == nil) {
        return [DDTrackingConsent granted];
    }
    
    if ([consent isPending] == YES) {
        return [DDTrackingConsent pending];
    }
    
    if ([consent getConsentAdStorage] == YES) {
        return [DDTrackingConsent granted];
    }
    
    return [DDTrackingConsent notGranted];
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    NSString * AppleDatadogApplicationDelegate_ClientToken = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"ClientToken" withDefault:nil];
    
    if (AppleDatadogApplicationDelegate_ClientToken == nil) {
        IOS_LOGGER_ERROR(@"plugin %@ ClientToken not found", @PLUGIN_BUNDLE_NAME);
        
        return NO;
    }
    
    DDConfiguration * configuration = [[DDConfiguration alloc] initWithClientToken:AppleDatadogApplicationDelegate_ClientToken
#if defined(MENGINE_DEBUG)
                                                                               env:@"dev"
#else
                                                                               env:@"prod"
#endif
    ];
    
    NSString * AppleDatadogApplicationDelegate_Site = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"Site" withDefault:@"us1"];
    
    if ([AppleDatadogApplicationDelegate_Site isEqual:@"us1"] == YES) {
        configuration.site = [DDSite us1];
    } else if ([AppleDatadogApplicationDelegate_Site isEqual:@"us3"] == YES) {
        configuration.site = [DDSite us3];
    } else if ([AppleDatadogApplicationDelegate_Site isEqual:@"eu1"] == YES) {
        configuration.site = [DDSite eu1];
    } else {
        IOS_LOGGER_ERROR(@"plugin %@ Site %@ not found", @PLUGIN_BUNDLE_NAME, AppleDatadogApplicationDelegate_Site);
        
        return NO;
    }
    
    iOSTransparencyConsentParam * consent = [[iOSTransparencyConsentParam alloc] initFromUserDefaults];
    
    DDTrackingConsent * trackingConsent = [self getTrackingConsent:consent];
        
    [DDDatadog initializeWithConfiguration:configuration
                           trackingConsent:trackingConsent];
    
    NSString * userId = [[iOSApplication sharedInstance] getUserId];
    NSString * installId = [[iOSApplication sharedInstance] getInstallId];
    
    [DDDatadog setUserInfoWithUserId:userId name:nil email:nil extraInfo:@{@"install_id": installId}];
    
    DDLogsConfiguration * logsConfiguration = [[DDLogsConfiguration alloc] initWithCustomEndpoint:nil];
    
    [DDLogs enableWith:logsConfiguration];
    
    DDLoggerConfiguration * loggerConfiguration = [[DDLoggerConfiguration alloc]
                                                   initWithService:nil
                                                   name:nil
                                                   networkInfoEnabled:NO
                                                   bundleWithRumEnabled:NO
                                                   bundleWithTraceEnabled:NO
                                                   remoteSampleRate:100
                                                   remoteLogThreshold:DDLogLevelDebug
                                                   printLogsToConsole:NO];
    
    DDLogger * logger = [DDLogger createWith:loggerConfiguration];
       
    [logger addAttributeForKey:@"mng_base" value:@{
        @"build.debug": MENGINE_DEBUG_VALUE(@(YES), @(NO)),
        @"version.code": [iOSDetail getBuildNumber],
        @"version.name": [iOSDetail getBuildVersion],
        @"install.id": [[iOSApplication sharedInstance] getInstallId],
        @"install.timestamp": @([[iOSApplication sharedInstance] getInstallTimestamp]),
        @"install.version": [[iOSApplication sharedInstance] getInstallVersion],
        @"install.rnd": @([[iOSApplication sharedInstance] getInstallRND]),
        @"session.index": @([[iOSApplication sharedInstance] getSessionIndex]),
        @"session.timestamp": @([[iOSApplication sharedInstance] getSessionTimestamp]),
        @"session.rnd": @([[iOSApplication sharedInstance] getSessionRND]),
        @"device.model": [iOSDetail getDeviceModel],
    }];
    
    self.m_logger = logger;
    
    return YES;
}

#pragma mark - iOSPluginUserIdDelegateInterface

- (void)onUserId:(iOSUserParam *)user {
    NSString * installId = [[iOSApplication sharedInstance] getInstallId];
    
    [DDDatadog setUserInfoWithUserId:user.USER_ID name:nil email:nil extraInfo:@{@"install_id": installId}];
}

- (void)onRemoveUserData {
    [DDDatadog setUserInfoWithUserId:@"" name:nil email:nil extraInfo:@{}];
    
    [DDDatadog clearAllData];
}

#pragma mark - iOSPluginLoggerDelegateInterface

- (void)onLogger:(AppleLogRecordParam * _Nonnull)record {
    if (self.m_logger == nil) {
        return;
    }
    
#ifdef MENGINE_DEBUG
    NSDictionary * attributes = @{ @"mng_record": @{
            @"code.category": record.LOG_CATEGORY,
            @"code.thread": record.LOG_THREAD,
            @"code.file": record.LOG_FILE,
            @"code.line": @(record.LOG_LINE),
            @"code.function": record.LOG_FUNCTION
        }
    };
#else
    NSDictionary * attributes = @{ @"mng_record": @{
            @"code.category": record.LOG_CATEGORY,
            @"code.thread": record.LOG_THREAD,
        }
    };
#endif
    
    switch (record.LOG_LEVEL) {
        case Mengine::LM_SILENT:
        case Mengine::LM_VERBOSE:
        case Mengine::LM_DEBUG:
        {
            //Empty
        }break;
        case Mengine::LM_INFO:
        {
            if (self.m_enableInfoMessage == NO) {
                return;
            }
            
            [self.m_logger info:record.LOG_DATA attributes:attributes];
        }break;
        case Mengine::LM_MESSAGE:
        case Mengine::LM_MESSAGE_RELEASE:
        {
            if (self.m_enableDebugMessage == NO) {
                return;
            }
            
            [self.m_logger info:record.LOG_DATA attributes:attributes];
        }break;
        case Mengine::LM_WARNING:
        {
            [self.m_logger warn:record.LOG_DATA attributes:attributes];
        }break;
        case Mengine::LM_ERROR:
        {
            [self.m_logger error:record.LOG_DATA attributes:attributes];
        }break;
        case Mengine::LM_FATAL:
        {
            [self.m_logger critical:record.LOG_DATA attributes:attributes];
        }break;
    }
}

#pragma mark - iOSPluginConfigDelegateInterface

- (void)onConfig:(NSDictionary * _Nonnull)config {
#ifdef MENGINE_RELEASE
    self.m_enableDebugMessage = [[config objectForKey:@"datadog_debug_message"] boolValue];
#endif
    
    self.m_enableInfoMessage = [[config objectForKey:@"datadog_info_message"] boolValue];
}

#pragma mark - iOSPluginTransparencyConsentDelegateInterface

- (void)onTransparencyConsent:(iOSTransparencyConsentParam *)consent {
    DDTrackingConsent * trackingConsent = [self getTrackingConsent:consent];
    
    [DDDatadog setTrackingConsentWithConsent:trackingConsent];
}

@end
