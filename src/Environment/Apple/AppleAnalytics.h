#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@interface AppleAnalytics : NSObject

+ (void)name:(NSString*)eventName params:(NSDictionary<NSString*, id> *)params;

@end
