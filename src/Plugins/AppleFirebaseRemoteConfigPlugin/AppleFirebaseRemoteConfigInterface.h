#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@protocol AppleFirebaseRemoteConfigInterface <NSObject>

+ (instancetype) sharedInstance;

- (BOOL)hasRemoteConfig:(NSString *)key
- (NSDictionary *)getRemoteConfigValue:(NSString *)key;

@end
