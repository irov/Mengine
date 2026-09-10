#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "Environment/iOS/iOSPluginUserIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginAdRevenueDelegateInterface.h"
#import "Environment/iOS/iOSPluginTransparencyConsentDelegateInterface.h"
#import "Environment/iOS/iOSPluginAnalyticDelegateInterface.h"
#import "Environment/iOS/iOSPluginAttributionDelegateInterface.h"

@interface iOSFirebaseAnalyticsPlugin : NSObject<iOSPluginInterface, iOSPluginAdRevenueDelegateInterface, iOSPluginTransparencyConsentDelegateInterface, iOSPluginUserIdDelegateInterface, iOSPluginAnalyticDelegateInterface, iOSPluginAttributionDelegateInterface>

+ (instancetype)sharedInstance;

- (void)sendEvent:(NSString *)name parameters:(NSDictionary<NSString *, id> *)parameters;

@property (nonatomic, assign) BOOL m_analyticsEnabled;

@end
