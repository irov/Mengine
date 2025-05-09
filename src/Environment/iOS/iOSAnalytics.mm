#import "iOSAnalytics.h"

#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSDetail.h"

@implementation iOSAnalytics

+ (void)event:(NSString * _Nonnull)event params:(NSDictionary * _Nonnull)params {
    [AppleDetail addMainQueueOperation:^{
        NSObject<iOSUIMainApplicationDelegateInterface> * delegate = [iOSDetail getUIMainApplicationDelegate];
        
        [delegate eventAnalytic:event params:params];
    }];
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
