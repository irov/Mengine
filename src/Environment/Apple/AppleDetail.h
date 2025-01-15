#pragma once

#include "Kernel/Params.h"
#include "Kernel/Data.h"
#include "Kernel/VectorString.h"
#include "Kernel/VectorConstString.h"

#import <Foundation/Foundation.h>

#ifndef MENGINE_APPLE_MESSAGE_NSID
#define MENGINE_APPLE_MESSAGE_NSID 1024
#endif

#ifndef MENGINE_APPLE_MESSAGE_NSERROR
#define MENGINE_APPLE_MESSAGE_NSERROR 4096
#endif

@interface AppleDetail : NSObject

+ (NSString * _Nonnull) NSIdToString:(id _Nonnull)value;
+ (NSString * _Nonnull) getMessageFromNSError:(NSError * _Nonnull)error;

+ (NSInteger) getCurrentTimeMillis;
+ (NSInteger) getSecureRandomInteger;
+ (NSString * _Nonnull) getRandomHexString:(NSInteger)length;

+ (BOOL) hasOption:(NSString * _Nonnull)value;

+ (BOOL) isValidJSONString:(NSString * _Nonnull)value;

+ (void) getParamsFromNSDictionary:(NSDictionary * _Nonnull)_in outParams:(Mengine::Params * const _Nonnull)_out;
+ (BOOL) getParamsFromJSON:(NSString * _Nonnull)_in outParams:(Mengine::Params * const _Nonnull)_out;
+ (NSDictionary * _Nonnull)getNSDictionaryFromParams:(const Mengine::Params &)params;
+ (void) getVectorStringFromNSArray:(NSArray<NSString *> * _Nonnull)_in outVectorString:(Mengine::VectorString * const _Nonnull)_out;
+ (NSArray<NSString *> * _Nonnull)getNSArrayFromVectorString:(const Mengine::VectorString &)strings;
+ (void) getVectorConstStringFromNSArray:(NSArray<NSString *> * _Nonnull)_in outVectorConstString:(Mengine::VectorConstString * const _Nonnull)_out;
+ (NSArray<NSString *> * _Nonnull)getNSArrayFromVectorConstString:(const Mengine::VectorConstString &)strings;
+ (void) getDataFromNSData:(NSData * _Nonnull)_in outData:(Mengine::Data * const _Nonnull)_out;
+ (NSData * _Nonnull) getNSDataFromData:(const Mengine::Data &)data;

+ (void) raisePureVirtualMethodException:(Class _Nonnull)klass selector:(SEL _Nonnull)selector;

@end
