#pragma once

#include "Kernel/StaticString.h"
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

typedef Mengine::StaticString<MENGINE_APPLE_MESSAGE_NSID> NSIDMessage;
+ (NSIDMessage)NSIdToString:(id _Nonnull) _value;

typedef Mengine::StaticString<MENGINE_APPLE_MESSAGE_NSERROR> NSErrorMessage;
+ (NSErrorMessage)getMessageFromNSError:(NSError * _Nonnull) _error;

+ (NSInteger)getCurrentTimeMillis;
+ (NSInteger)getSecureRandomInteger;
+ (NSString * _Nonnull)getRandomHexString:(NSInteger) _length;

+ (BOOL)isValidJSONString:(NSString * _Nonnull) _value;

+ (void)getParamsFromNSDictionary:(NSDictionary * _Nonnull) _in outParams:(Mengine::Params * const _Nonnull) _out;
+ (NSDictionary * _Nonnull)getNSDictionaryFromParams:(const Mengine::Params &) _params;
+ (void)getVectorStringFromNSArray:(NSArray<NSString *> * _Nonnull) _in outVectorString:(Mengine::VectorString * const _Nonnull) _out;
+ (NSArray<NSString *> * _Nonnull)getNSArrayFromVectorString:(const Mengine::VectorString &) _strings;
+ (void)getVectorConstStringFromNSArray:(NSArray<NSString *> * _Nonnull) _in outVectorConstString:(Mengine::VectorConstString * const _Nonnull) _out;
+ (NSArray<NSString *> * _Nonnull)getNSArrayFromVectorConstString:(const Mengine::VectorConstString &) _strings;
+ (void)getDataFromNSData:(NSData * _Nonnull) _in outData:(Mengine::Data * const _Nonnull) _out;
+ (NSData * _Nonnull)getNSDataFromData:(const Mengine::Data &) _data;

+ (void)raisePureVirtualMethodException:(Class _Nonnull)klass selector:(SEL _Nonnull)selector;

@end
