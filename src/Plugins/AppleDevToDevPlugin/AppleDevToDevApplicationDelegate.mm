#import "AppleDevToDevApplicationDelegate.h"

#import "Environment/Apple/AppleBundle.h"

#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#import <DTDAnalytics/DTDAnalytics-Swift.h>

@implementation AppleDevToDevApplicationDelegate

- (void)sendEvent:(NSString *)eventName parameters:(NSDictionary<NSString *, id> *)parameters {
    IOS_LOGGER_MESSAGE( @"sendEvent name: %@ parameters: %@"
        , eventName
        , [NSString stringWithFormat:@"%@", parameters]
    );

    DTDCustomEventParameters * devtodev_parameters = [[DTDCustomEventParameters alloc] init];

    for (NSString * key in parameters)
    {
        id value = parameters[key];
        
        [devtodev_parameters addString:key value:value];
    }

    [DTDAnalytics customEvent:eventName withParameters:devtodev_parameters];
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if ([AppleBundle hasPluginConfig:@"MengineAppleDevToDevPlugin"] == NO) {
        return NO;
    }
    
    NSString * MengineAppleDevToDevPlugin_AppKey = [AppleBundle getPluginConfigString:@"MengineAppleDevToDevPlugin" withKey:@"AppKey" withDefault:nil];

    DTDAnalyticsConfiguration * config = [[DTDAnalyticsConfiguration alloc] init];

#ifdef MENGINE_DEBUG
    config.logLevel = DTDLogLevelWarning;
#else
    config.logLevel = DTDLogLevelError;
#endif
    
    NSString * userId = [iOSApplication.sharedInstance getUserId];
    
    config.userId = userId;

    [DTDAnalytics applicationKey:MengineAppleDevToDevPlugin_AppKey configuration:config];
    
    [DTDAnalytics trackingAvailabilityHandler:^(BOOL value) {
        IOS_LOGGER_MESSAGE( @"initialized has been finished [%@]"
            , (value == YES ? @"SUCCESSFUL" : @"FAILED")
        );
            
        //Empty
    }];
    
    return YES;
}

#pragma mark - iOSPluginUserIdDelegateInterface

- (void)onUserId:(iOSUserParam *)user {
    [DTDAnalytics userId:user.USER_ID];
}

- (void)onRemoveUserData {
    //Empty
}

#pragma mark - iOSPluginAnalyticDelegateInterface

- (void)onAnalyticEvent:(NSString *)event params:(NSDictionary *)params {
    [self sendEvent:event parameters:params];
}

- (void)onAnalyticScreen:(NSString *)screen type:(NSString *)type {
    DTDCustomEventParameters * devtodev_parameters = [[DTDCustomEventParameters alloc] init];
    
    [devtodev_parameters addString:@"screen_type" value:type];
    [devtodev_parameters addString:@"screen_name" value:screen];
    
    [DTDAnalytics customEvent:@"screen_view" withParameters:devtodev_parameters];
}

- (void)onAnalyticFlush {
    // DevToDev automatically flushes events, no manual flush needed
}

@end
