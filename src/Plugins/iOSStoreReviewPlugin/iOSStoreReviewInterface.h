#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol iOSStoreReviewInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)launchTheInAppReview;

@end
