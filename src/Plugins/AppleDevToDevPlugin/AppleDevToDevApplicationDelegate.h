#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginUserIdDelegateInterface.h"

@interface AppleDevToDevApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, iOSPluginUserIdDelegateInterface>

- (void)sendEvent:(NSString *)eventName parameters:(NSDictionary<NSString *, id> *)parameters;

@end
