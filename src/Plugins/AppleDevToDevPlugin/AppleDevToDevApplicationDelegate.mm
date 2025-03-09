#import "AppleDevToDevApplicationDelegate.h"

#import "Environment/iOS/iOSDetail.h"

#import <DTDAnalytics/DTDAnalytics-Swift.h>

@implementation AppleDevToDevApplicationDelegate

- (void)sendEvent:(NSString *)eventName parameters:(NSDictionary<NSString *, id> *)parameters {
        IOS_LOGGER_MESSAGE( @"sendEvent name: %@ parameters: %@"
            , _eventName
            , [NSString stringWithFormat:@"%@", _parameters]
        );

        DTDCustomEventParameters * devtodev_parameters = [[DTDCustomEventParameters alloc] init];

        for (NSString * key in _parameters)
        {
            id value = _parameters[key];
            
            [devtodev_parameters addString:key value:value];
        }

        [DTDAnalytics customEvent:_eventName withParameters:devtodev_parameters];
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if (Mengine::Helper::iOSHasBundlePluginConfig(@"MengineAppleDevToDevPlugin") == NO) {
        return NO;
    }
    
    NSString * MengineAppleDevToDevPlugin_AppKey = Mengine::Helper::AppleGetBundlePluginConfigString(@"MengineAppleDevToDevPlugin", @"AppKey", nil);

    DTDAnalyticsConfiguration * config = [[DTDAnalyticsConfiguration alloc] init];

#ifdef MENGINE_DEBUG
    config.logLevel = DTDLogLevelDebug;
#else
    config.logLevel = DTDLogLevelError;
#endif

    [DTDAnalytics applicationKey:MengineAppleDevToDevPlugin_AppKey configuration:config];
    
    [DTDAnalytics trackingAvailabilityHandler:^(BOOL value) {
        IOS_LOGGER_MESSAGE( @"initialized has been finished [@s]"
            , (value == YES ? @"SUCCESSFUL" : @"FAILED")
        );
            
        //Empty
    }];
    
    return YES;
}

@end
