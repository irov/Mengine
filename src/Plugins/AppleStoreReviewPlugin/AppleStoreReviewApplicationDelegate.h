#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

@interface AppleStoreReviewApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface>

+ (instancetype) sharedInstance;

- (void)launchTheInAppReview;

@end
