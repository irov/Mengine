#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@protocol AppleFirebaseCrashlyticsInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)log:(NSString *)message;
- (void)setCustomValue:(NSString *)value forKey:(NSString *)key;
- (void)setCustomKeysAndValues:(NSDictionary *)keysAndValues;
- (void)recordError:(NSString *)name errorCode:(NSInteger)code;
- (void)recordError:(NSError *)error;

@end
