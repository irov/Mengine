#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol AppleUserMessagingPlatformInterface <NSObject>

+ (instancetype)sharedInstance;

- (BOOL)isComplete;

@end
