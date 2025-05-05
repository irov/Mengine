#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@interface AppleKeyChain : NSObject

+ (BOOL)setDataForKey:(NSString *)key data:(NSData *)data;
+ (NSData *)getDataForKey:(NSString *)key;

+ (BOOL)setStringForKey:(NSString *)key value:(NSString *)value;
+ (NSString *)getStringForKey:(NSString *)key defaultValue:(NSString *)defaultValue;

+ (BOOL)setBooleanForKey:(NSString *)key value:(BOOL)value;
+ (BOOL)getBooleanForKey:(NSString *)key defaultValue:(BOOL)defaultValue;

+ (BOOL)setIntegerForKey:(NSString *)key value:(NSInteger)value;
+ (NSInteger)getIntegerForKey:(NSString *)key defaultValue:(NSInteger)defaultValue;

+ (BOOL)setDoubleForKey:(NSString *)key value:(double)value;
+ (double)getDoubleForKey:(NSString *)key defaultValue:(double)defaultValue;

+ (BOOL)setNSTimeIntervalForKey:(NSString *)key value:(NSTimeInterval)value;
+ (NSTimeInterval)getNSTimeIntervalForKey:(NSString *)key defaultValue:(NSTimeInterval)defaultValue;

+ (BOOL)hasKey:(NSString *)key;
+ (BOOL)removeKey:(NSString *)key;

+ (BOOL)clear;

@end
