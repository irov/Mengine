#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginSessionIdDelegateInterface.h"

@interface AppleDevToDevApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, iOSPluginSessionIdDelegateInterface>

- (void)sendEvent:(NSString *)eventName parameters:(NSDictionary<NSString *, id> *)parameters;

@end
