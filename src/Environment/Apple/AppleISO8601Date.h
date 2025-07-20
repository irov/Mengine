#pragma once

#import "Environment/Apple/AppleIncluder.h"

@interface AppleISO8601Date : NSObject

+ (NSTimeInterval)NSTimeIntervalFromISO8601Duration:(NSString *)date;

@end
