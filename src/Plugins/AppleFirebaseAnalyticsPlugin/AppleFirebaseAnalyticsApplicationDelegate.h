#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginUserIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginAdRevenueDelegateInterface.h"
#import "Environment/iOS/iOSPluginTransparencyConsentDelegateInterface.h"

@interface AppleFirebaseAnalyticsApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, iOSPluginAdRevenueDelegateInterface, iOSPluginTransparencyConsentDelegateInterface, iOSPluginUserIdDelegateInterface>

+ (instancetype)sharedInstance;

- (void)sendEvent:(NSString *)name parameters:(NSDictionary<NSString *, id> *)parameters;

@end


