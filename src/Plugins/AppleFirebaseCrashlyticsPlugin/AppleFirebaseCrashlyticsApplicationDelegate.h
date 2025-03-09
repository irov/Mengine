#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginSessionIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginLoggerDelegateInterface.h"

@interface AppleFirebaseCrashlyticsApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, iOSPluginLoggerDelegateInterface, iOSPluginSessionIdDelegateInterface>

+ (instancetype) sharedInstance;

- (void)log:(NSString *)message;
- (void)setCustomValue:(NSString *)value forKey:(NSString *)key;
- (void)setCustomKeysAndValues:(NSDictionary *)keysAndValues;
- (void)recordError:(NSString *)name errorCode:(NSInteger)code;
- (void)recordError:(NSError *)error;

@end
