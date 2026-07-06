#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol iOSUserMessagingPlatformInterface <NSObject>

+ (instancetype)sharedInstance;

- (BOOL)isComplete;

@end
