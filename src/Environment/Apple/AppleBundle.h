#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@interface AppleBundle : NSObject

+ (NSString *) getIdentifier;

+ (BOOL)hasPluginConfig:(NSString *) _plugin;
+ (NSDictionary *)getPluginConfig:(NSString *) _plugin;
+ (NSString *)getPluginConfigString:(NSString *) _plugin withKey:(NSString *) _key withDefault:(NSString *) _default;
+ (NSNumber *)getPluginConfigNumber:(NSString *) _plugin withKey:(NSString *) _key withDefault:(NSNumber *) _default;
+ (NSInteger)getPluginConfigInteger:(NSString *) _plugin withKey:(NSString *) _key withDefault:(NSInteger) _default;
+ (double)getPluginConfigDouble:(NSString *) _plugin withKey:(NSString *) _key withDefault:(double) _default;
+ (BOOL)getPluginConfigBoolean:(NSString *) _plugin withKey:(NSString *) _key withDefault:(BOOL) _default;

@end
