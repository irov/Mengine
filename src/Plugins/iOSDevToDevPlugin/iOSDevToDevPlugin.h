#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "Environment/iOS/iOSPluginUserIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginAnalyticDelegateInterface.h"

@interface iOSDevToDevPlugin : NSObject<iOSPluginInterface, iOSPluginUserIdDelegateInterface, iOSPluginAnalyticDelegateInterface>

- (void)sendEvent:(NSString *)eventName parameters:(NSDictionary<NSString *, id> *)parameters;

@end
