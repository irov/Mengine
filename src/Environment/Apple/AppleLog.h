#pragma once

#import "Environment/Apple/AppleLogRecordParam.h"

@interface AppleLog : NSObject

+ (void) withFormat:(NSString * _Nonnull)format, ... NS_FORMAT_FUNCTION(1, 2);
+ (void) withFormatV:(NSString * _Nonnull)format arguments:(va_list)args;

+ (void) withRecord:(AppleLogRecordParam * _Nonnull)record;

@end
