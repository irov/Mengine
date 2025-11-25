#import "AppleAmplitudeApplicationDelegate.h"

#import "Environment/Apple/AppleBundle.h"
#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSAdRevenueParam.h"
#import "Environment/iOS/iOSLog.h"
#import "Environment/iOS/iOSUserParam.h"

#import <Amplitude/Amplitude.h>

#define PLUGIN_BUNDLE_NAME "MengineAppleAmplitudePlugin"

@implementation AppleAmplitudeApplicationDelegate

#pragma mark - UIApplicationDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    MENGINE_UNUSED( application );
    MENGINE_UNUSED( launchOptions );

    if ([AppleBundle hasPluginConfig:@PLUGIN_BUNDLE_NAME] == NO) {
        IOS_LOGGER_ERROR( @"[AppleAmplitude] plugin config '%s' not found", PLUGIN_BUNDLE_NAME );
        
        return NO;
    }

    NSString * apiKey = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"ApiKey" withDefault:nil];

    if (apiKey == nil || apiKey.length == 0) {
        IOS_LOGGER_ERROR( @"[AppleAmplitude] plugin config '%s.ApiKey' is not specified", PLUGIN_BUNDLE_NAME );
        
        return NO;
    }
    
    NSString * userId = [iOSApplication.sharedInstance getUserId];

    [[Amplitude instance] initializeApiKey:apiKey userId:userId];

    NSString * installId = [iOSApplication.sharedInstance getInstallId];

    [[Amplitude instance] setDeviceId:installId];

    NSInteger installTimestamp = [iOSApplication.sharedInstance getInstallTimestamp];
    NSString * installVersion = [iOSApplication.sharedInstance getInstallVersion];
    NSInteger installRND = [iOSApplication.sharedInstance getInstallRND];
    NSInteger sessionIndex = [iOSApplication.sharedInstance getSessionIndex];
    NSInteger sessionTimestamp = [iOSApplication.sharedInstance getSessionTimestamp];

    AMPIdentify * identify = [AMPIdentify identify];

    [identify set:@"is_publish" value:MENGINE_BUILD_PUBLISH_VALUE(@(YES), @(NO))];
    [identify set:@"is_debug" value:MENGINE_DEBUG_VALUE(@(YES), @(NO))];
    [identify set:@"install_id" value:installId];
    [identify set:@"install_timestamp" value:@(installTimestamp)];
    [identify set:@"install_version" value:installVersion];
    [identify set:@"install_rnd" value:@(installRND)];
    [identify set:@"session_index" value:@(sessionIndex)];
    [identify set:@"session_timestamp" value:@(sessionTimestamp)];

    NSInteger currentTimestamp = [AppleDetail getTimestamp];
    NSInteger lifeTime = currentTimestamp - installTimestamp;

    [identify set:@"life_time" value:@(lifeTime)];

    [[Amplitude instance] identify:identify];

    return YES;
}

#pragma mark - iOSPluginUserIdDelegateInterface

- (void)onUserId:(iOSUserParam *)user {
    [[Amplitude instance] setUserId:user.USER_ID];
}

- (void)onRemoveUserData {
    Amplitude * amplitude = [Amplitude instance];
    
    [amplitude setUserId:nil];
    [amplitude setDeviceId:[iOSApplication.sharedInstance getInstallId]];
}

#pragma mark - iOSPluginAdRevenueDelegateInterface

- (void)onAdRevenue:(iOSAdRevenueParam *)revenue {
    AMPRevenue * ampRevenue = [AMPRevenue revenue];
    
    [ampRevenue setQuantity:1];
    [ampRevenue setPrice:revenue.REVENUE_VALUE];

    if (revenue.REVENUE_FORMAT != nil) {
        [ampRevenue setRevenueType:revenue.REVENUE_FORMAT];
    }

    NSMutableDictionary<NSString *, id> * properties = [NSMutableDictionary dictionary];

    if (revenue.REVENUE_PLATFORM != nil) {
        properties[@"ad_platform"] = revenue.REVENUE_PLATFORM;
    }

    if (revenue.REVENUE_SOURCE != nil) {
        properties[@"ad_source"] = revenue.REVENUE_SOURCE;
    }

    if (revenue.REVENUE_FORMAT != nil) {
        properties[@"ad_format"] = revenue.REVENUE_FORMAT;
    }

    if (revenue.REVENUE_UNIT != nil) {
        properties[@"ad_unit_id"] = revenue.REVENUE_UNIT;
    }

    if (revenue.REVENUE_PLACEMENT != nil) {
        properties[@"placement"] = revenue.REVENUE_PLACEMENT;
    }

    if (revenue.REVENUE_NETWORK_PLACEMENT != nil) {
        properties[@"network_placement"] = revenue.REVENUE_NETWORK_PLACEMENT;
    }

    if (revenue.REVENUE_COUNTRY_CODE != nil) {
        properties[@"country_code"] = revenue.REVENUE_COUNTRY_CODE;
    }

    if (revenue.REVENUE_CURRENCY != nil) {
        properties[@"currency"] = revenue.REVENUE_CURRENCY;
    }

    if (properties.count != 0) {
        [ampRevenue setEventProperties:properties];
    }

    [[Amplitude instance] logRevenueV2:ampRevenue];
}

#pragma mark - iOSPluginAnalyticDelegateInterface

- (void)onAnalyticEvent:(NSString *)event params:(NSDictionary *)params {
    [[Amplitude instance] logEvent:event withEventProperties:params];
}

- (void)onAnalyticScreen:(NSString *)screen type:(NSString *)type {
    [[Amplitude instance] logEvent:@"screen_view" withEventProperties:@{
        @"screen_name": screen,
        @"screen_type": type
    }];
}

- (void)onAnalyticFlush {
    [[Amplitude instance] uploadEvents];
}

@end

