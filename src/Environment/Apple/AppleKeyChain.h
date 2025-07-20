#pragma once

#import "Environment/Apple/AppleIncluder.h"

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

+ (BOOL)setTimeIntervalForKey:(NSString *)key value:(NSTimeInterval)value;
+ (NSTimeInterval)getTimeIntervalForKey:(NSString *)key defaultValue:(NSTimeInterval)defaultValue;

+ (BOOL)setSetForKey:(NSString *)key value:(NSSet *)value;
+ (NSSet *)getSetForKey:(NSString *)key;

+ (BOOL)hasKey:(NSString *)key;
+ (BOOL)removeKey:(NSString *)key;

+ (BOOL)clear;

@end
