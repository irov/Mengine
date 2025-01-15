#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@interface AppleAnalytics : NSObject

+ (void) event:(NSString*)eventName params:(NSDictionary<NSString*, id> *)params;

@end
