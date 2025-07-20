#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol AppleFirebaseRemoteConfigInterface <NSObject>

+ (instancetype)sharedInstance;

- (BOOL)hasRemoteConfig:(NSString *)key;
- (NSDictionary *)getRemoteConfigValue:(NSString *)key;

@end
