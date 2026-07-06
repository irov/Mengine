#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol iOSFirebaseRemoteConfigInterface <NSObject>

+ (instancetype)sharedInstance;

- (BOOL)hasRemoteConfig:(NSString *)key;
- (NSDictionary *)getRemoteConfigValue:(NSString *)key;

@end
