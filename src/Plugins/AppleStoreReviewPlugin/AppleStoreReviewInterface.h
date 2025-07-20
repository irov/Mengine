#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol AppleStoreReviewInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)launchTheInAppReview;

@end
