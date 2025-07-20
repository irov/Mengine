#pragma once

#import "Environment/Apple/AppleIncluder.h"

#include "Kernel/String.h"
#include "Kernel/ConstString.h"

@interface AppleString : NSObject

+ (size_t)NSStringCopyToString:(NSString * _Nonnull) _src outString:(Mengine::Char * const _Nonnull) _dst withCapacity:(size_t) _capacity;
    
+ (Mengine::ConstString)NSStringToConstString:(NSString * _Nonnull) _value;
+ (Mengine::String)NSStringToString:(NSString * _Nonnull) _value;

+ (NSString * _Nonnull)NSStringFromString:(const Mengine::Char * _Nonnull) _value withSize:(size_t) _size;
+ (NSString * _Nonnull)NSStringFromUnicode:(const Mengine::WString &) _value;
+ (NSString * _Nonnull)NSStringFromString:(const Mengine::String &) _value;
+ (NSString * _Nonnull)NSStringFromConstString:(const Mengine::ConstString &) _value;

@end
