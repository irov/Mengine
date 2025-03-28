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

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    NSString * AppleDatadogApplicationDelegate_ClientToken = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"ClientToken" withDefault:nil];
    
    if (AppleDatadogApplicationDelegate_ClientToken == nil) {
        IOS_LOGGER_ERROR(@"plugin %@ ClientToken not found", @PLUGIN_BUNDLE_NAME);
        
        return NO;
    }
    
    DDConfiguration * configuration = [[DDConfiguration alloc] initWithClientToken:AppleDatadogApplicationDelegate_ClientToken
#ifdef DEBUG
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
    
    [DDDatadog initializeWithConfiguration:configuration
                           trackingConsent:[DDTrackingConsent granted]];
    
    NSString * sessionId = [[iOSApplication sharedInstance] getSessionId];
    NSString * installKey = [[iOSApplication sharedInstance] getInstallKey];
    
    [DDDatadog setUserInfoWithUserId:sessionId name:nil email:nil extraInfo:@{@"install_key":installKey}];
    
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
        
    [logger addAttributeForKey:@"version" value:@{
        @"number": [iOSDetail getBuildNumber],
        @"version": [iOSDetail getBuildVersion]
    }];
        
    [logger addAttributeForKey:@"install" value:@{
        @"key": [[iOSApplication sharedInstance] getInstallKey],
        @"timestamp": @([[iOSApplication sharedInstance] getInstallTimestamp]),
        @"version": [[iOSApplication sharedInstance] getInstallVersion],
        @"rnd": @([[iOSApplication sharedInstance] getInstallRND])
    }];
    
    [logger addAttributeForKey:@"session" value:@{
        @"index": @([[iOSApplication sharedInstance] getSessionIndex]),
        @"timestamp": @([[iOSApplication sharedInstance] getSessionTimestamp]),
    }];
        
    [logger addAttributeForKey:@"device" value:@{
        @"model": [iOSDetail getDeviceModel]
    }];
    
    [logger addAttributeForKey:@"os" value:@{
        @"family": @"iOS",
        @"version": [iOSDetail getSystemVersion]
    }];
    
    self.m_logger = logger;
    
    return YES;
}

- (void)onSessionId:(iOSSessionIdParam *)session {
    NSString * installKey = [[iOSApplication sharedInstance] getInstallKey];
    
    [DDDatadog setUserInfoWithUserId:session.SESSION_ID name:nil email:nil extraInfo:@{@"install_key":installKey}];
}
- (void)onRemoveSessionData {
    [DDDatadog setUserInfoWithUserId:@"" name:nil email:nil extraInfo:@{}];
}

- (void)onLogger:(AppleLogRecordParam * _Nonnull)record {
    if (self.m_logger == nil) {
        return;
    }
    
    NSDictionary * attributes = @{
        @"category": record.LOG_CATEGORY,
        @"thread": record.LOG_THREAD,
        @"code": @{
            @"file": record.LOG_FILE,
            @"line": @(record.LOG_LINE),
            @"function": record.LOG_FUNCTION
        }
    };
    
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

- (void)onConfig:(NSDictionary * _Nonnull)config {
#ifdef MENGINE_RELEASE
    self.m_enableDebugMessage = [[config objectForKey:@"datadog_debug_message"] boolValue];
#endif
    
    self.m_enableInfoMessage = [[config objectForKey:@"datadog_info_message"] boolValue];
}

@end
