#pragma once

#include "Config/Char.h"
#include "Config/StdDef.h"

#import <Foundation/Foundation.h>

@interface AppleUserDefaults : NSObject

+ (NSString *)getStringForKey:(NSString *)key defaultValue:(NSString *)defaultValue;
+ (BOOL)setStringForKey:(NSString *)key value:(NSString *)value;

+ (BOOL)getBooleanForKey:(NSString *)key defaultValue:(BOOL)defaultValue;
+ (BOOL)setBooleanForKey:(NSString *)key value:(BOOL)value;

+ (NSInteger)getIntegerForKey:(NSString *)key defaultValue:(NSInteger)defaultValue;
+ (BOOL)setIntegerForKey:(NSString *)key value:(NSInteger)value;

+ (BOOL)hasKey:(NSString *)key;
+ (BOOL)removeKey:(NSString *)key;

+ (BOOL)clear;

@end
