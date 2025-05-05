#pragma once

#import <Foundation/Foundation.h>

@interface AppleISO8601Date : NSObject

+ (NSTimeInterval)NSTimeIntervalFromISO8601Duration:(NSString *)date;

@end
