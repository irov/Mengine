#import "iOSAnalytics.h"

#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSAnalyticsEventCategory.h"

@implementation iOSAnalytics

+ (void)event:(NSString * _Nonnull)event category:(iOSAnalyticsEventCategory)category params:(NSDictionary * _Nonnull)params {
    [AppleDetail addMainQueueOperation:^{
        NSObject<iOSUIMainApplicationDelegateInterface> * delegate = [iOSDetail getUIMainApplicationDelegate];
        
        [delegate eventAnalytic:event category:category params:params];
    }];
}

+(void)eventSystem:(NSString * _Nonnull)event params:(NSDictionary * _Nonnull)params {
    [iOSAnalytics event:event category:iOSAnalyticsEventCategory_System params:params];
}

+(void)eventCustom:(NSString * _Nonnull)event params:(NSDictionary * _Nonnull)params {
    [iOSAnalytics event:event category:iOSAnalyticsEventCategory_Custom params:params];
}

+ (void)screen:(NSString * _Nonnull)name type:(NSString * _Nonnull)type {
    [AppleDetail addMainQueueOperation:^{
        NSObject<iOSUIMainApplicationDelegateInterface> * delegate = [iOSDetail getUIMainApplicationDelegate];
        
        [delegate eventAnalyticScreen:name type:type];
    }];
}

+ (void)flush {
    [AppleDetail addMainQueueOperation:^{
        NSObject<iOSUIMainApplicationDelegateInterface> * delegate = [iOSDetail getUIMainApplicationDelegate];
        
        [delegate eventAnalyticFlush];
    }];
}

@end
