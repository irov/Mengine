#import "iOSAnalytics.h"

#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSDetail.h"

@implementation iOSAnalytics

+ (void)analyticEvent:(NSString * _Nonnull)event params:(NSDictionary * _Nonnull)params {
    [AppleDetail addMainQueueOperation:^{
        NSObject<iOSUIMainApplicationDelegateInterface> *delegate = [iOSDetail getUIMainApplicationDelegate];
        
        [delegate analyticEvent:event params:params];
    }];
}

+ (void)analyticScreen:(NSString * _Nonnull)name type:(NSString * _Nonnull)type {
    [AppleDetail addMainQueueOperation:^{
        NSObject<iOSUIMainApplicationDelegateInterface> *delegate = [iOSDetail getUIMainApplicationDelegate];
        
        [delegate analyticScreen:name type:type];
    }];
}

+ (void)analyticFlush {
    [AppleDetail addMainQueueOperation:^{
        NSObject<iOSUIMainApplicationDelegateInterface> *delegate = [iOSDetail getUIMainApplicationDelegate];
        
        [delegate analyticFlush];
    }];
}

@end