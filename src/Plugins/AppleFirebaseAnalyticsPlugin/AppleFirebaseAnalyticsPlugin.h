#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "Environment/iOS/iOSPluginUserIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginAdRevenueDelegateInterface.h"
#import "Environment/iOS/iOSPluginTransparencyConsentDelegateInterface.h"
#import "Environment/iOS/iOSPluginAnalyticDelegateInterface.h"

@interface AppleFirebaseAnalyticsPlugin : NSObject<iOSPluginInterface, iOSPluginAdRevenueDelegateInterface, iOSPluginTransparencyConsentDelegateInterface, iOSPluginUserIdDelegateInterface, iOSPluginAnalyticDelegateInterface>

+ (instancetype)sharedInstance;

- (void)sendEvent:(NSString *)name parameters:(NSDictionary<NSString *, id> *)parameters;

@end


