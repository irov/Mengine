#pragma once

#include "Config/Config.h"

#import "Environment/iOS/iOSAdRevenueParam.h"

@protocol iOSPluginAdRevenueDelegateInterface <NSObject>

- (void)onAdRevenue:(iOSAdRevenueParam *)revenue;

@end
