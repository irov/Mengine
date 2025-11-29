#pragma once

#import "Environment/Apple/AppleIncluder.h"

#include "Kernel/Params.h"
#include "Kernel/Data.h"
#include "Kernel/VectorString.h"
#include "Kernel/VectorConstString.h"

#ifndef MENGINE_APPLE_MESSAGE_NSID
#define MENGINE_APPLE_MESSAGE_NSID 1024
#endif

#ifndef MENGINE_APPLE_MESSAGE_NSERROR
#define MENGINE_APPLE_MESSAGE_NSERROR 4096
#endif

@interface AppleDetail : NSObject

+ (NSString * _Nonnull)NSIdToString:(id _Nonnull)value;
+ (NSString * _Nonnull)getMessageFromNSError:(NSError * _Nonnull)error;

+ (NSInteger)getTimestamp;
+ (NSInteger)getSecureRandomInteger;
+ (NSString * _Nonnull)getRandomHexString:(NSUInteger)length;

+ (void)addMainQueueOperation:(dispatch_block_t _Nonnull)block;
+ (void)addMainQueueOperation:(dispatch_block_t _Nonnull)block afterSeconds:(NSTimeInterval)seconds;

+ (void)addDefaultQueueOperation:(dispatch_block_t _Nonnull)block;
+ (void)addBackgroundQueueOperation:(dispatch_block_t _Nonnull)block;
+ (void)addUtilityQueueOperation:(dispatch_block_t _Nonnull)block;
+ (void)addUserInitiatedQueueOperation:(dispatch_block_t _Nonnull)block;
+ (void)addUserInteractiveQueueOperation:(dispatch_block_t _Nonnull)block;

+ (void)cancelAllQueueOperations;

+ (BOOL)hasOption:(NSString * _Nonnull)value;

+ (BOOL)isValidJSONString:(NSString * _Nonnull)value;

+ (void)getParamsFromNSDictionary:(NSDictionary * _Nonnull)_in outParams:(Mengine::Params * const _Nonnull)_out;
+ (BOOL)getParamsFromJSON:(NSString * _Nonnull)_in outParams:(Mengine::Params * const _Nonnull)_out;
+ (NSDictionary * _Nonnull)getNSDictionaryFromParams:(const Mengine::Params &)params;
+ (void)getVectorStringFromNSArray:(NSArray<NSString *> * _Nonnull)_in outVectorString:(Mengine::VectorString * const _Nonnull)_out;
+ (NSArray<NSString *> * _Nonnull)getNSArrayFromVectorString:(const Mengine::VectorString &)strings;
+ (void)getVectorConstStringFromNSArray:(NSArray<NSString *> * _Nonnull)_in outVectorConstString:(Mengine::VectorConstString * const _Nonnull)_out;
+ (NSArray<NSString *> * _Nonnull)getNSArrayFromVectorConstString:(const Mengine::VectorConstString &)strings;
+ (void)getDataFromNSData:(NSData * _Nonnull)_in outData:(Mengine::Data * const _Nonnull)_out;
+ (NSData * _Nonnull)getNSDataFromData:(const Mengine::Data &)data;

+ (void)raisePureVirtualMethodException:(Class _Nonnull)klass selector:(SEL _Nonnull)selector;

+ (void)visitParameters:(NSDictionary<NSString *, id> * _Nonnull)parameters
                forBool:(void (^ _Nonnull)(NSString * _Nonnull key, BOOL value))forBool
             forInteger:(void (^ _Nonnull)(NSString * _Nonnull key, int64_t value))forInteger
              forDouble:(void (^ _Nonnull)(NSString * _Nonnull key, double value))forDouble
              forString:(void (^ _Nonnull)(NSString * _Nonnull key, NSString * _Nonnull value))forString
                forNull:(void (^ _Nonnull)(NSString * _Nonnull key))forNull
             forUnknown:(void (^ _Nonnull)(NSString * _Nonnull key, id _Nonnull value))forUnknown;

+ (void)visitValues:(NSSet<id> * _Nonnull)values
            forBool:(void (^ _Nonnull)(BOOL value))forBool
         forInteger:(void (^ _Nonnull)(int64_t value))forInteger
          forDouble:(void (^ _Nonnull)(double value))forDouble
          forString:(void (^ _Nonnull)(NSString * _Nonnull value))forString
            forNull:(void (^ _Nonnull)(void))forNull
         forUnknown:(void (^ _Nonnull)(id _Nonnull value))forUnknown;

+ (void)visitValues:(NSArray<id> * _Nonnull)values
            forBool:(void (^ _Nonnull)(BOOL value))forBool
         forInteger:(void (^ _Nonnull)(int64_t value))forInteger
          forDouble:(void (^ _Nonnull)(double value))forDouble
          forString:(void (^ _Nonnull)(NSString * _Nonnull value))forString
            forNull:(void (^ _Nonnull)(void))forNull
         forUnknown:(void (^ _Nonnull)(id _Nonnull value))forUnknown;

@end
