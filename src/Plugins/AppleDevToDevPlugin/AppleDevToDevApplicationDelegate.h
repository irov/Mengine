#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

@interface AppleDevToDevApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface>

- (void)sendEvent:(NSString *)eventName parameters:(NSDictionary<NSString *, id> *)parameters;

@end
