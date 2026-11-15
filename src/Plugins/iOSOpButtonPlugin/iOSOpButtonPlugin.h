#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "Environment/iOS/iOSPluginLoggerDelegateInterface.h"
#import "Environment/iOS/iOSPluginAnalyticDelegateInterface.h"
#import "Environment/iOS/iOSPluginAdRevenueDelegateInterface.h"

@interface iOSOpButtonPlugin : NSObject<iOSPluginInterface, iOSPluginLoggerDelegateInterface, iOSPluginAnalyticDelegateInterface, iOSPluginAdRevenueDelegateInterface>

+ (instancetype)sharedInstance;

- (BOOL)addCommandButton:(NSString *)tabName identifier:(NSString *)identifier title:(NSString *)title handler:(void (^)(void))handler;
- (void)removeCommandTab:(NSString *)tabName;

@end
