#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

- (void)appleAnalyticsEvent:(NSString*)eventName params:(NSDictionary<NSString*, id> *)params;