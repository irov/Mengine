#import "iOSAttribution.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSUIMainApplicationDelegateInterface.h"

@implementation iOSAttribution

+ (void)setAttribution:(NSString *)name value:(id)value {
    id attributionValue = value ?: NSNull.null;

    [AppleDetail addMainQueueOperation:^{
        NSObject<iOSUIMainApplicationDelegateInterface> * delegate = [iOSDetail getUIMainApplicationDelegate];

        [delegate eventAttribution:name value:attributionValue];
    }];
}

@end
