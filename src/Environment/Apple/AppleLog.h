#pragma once

#include "Config/StdArg.h"

#import <Foundation/Foundation.h>

@interface AppleLog : NSObject

+ (void)withFormat:(NSString *)format, ... NS_FORMAT_FUNCTION(1, 2);
+ (void)withFormatV:(NSString *)format arguments:(va_list)args;

@end
