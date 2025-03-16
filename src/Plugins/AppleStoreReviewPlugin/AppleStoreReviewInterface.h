#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@protocol AppleStoreReviewInterface <NSObject>

+ (instancetype) sharedInstance;

- (void)launchTheInAppReview;

@end
