#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol iOSFirebaseMessagingInterface <NSObject>

+ (instancetype _Nonnull)sharedInstance;

- (NSString * _Nullable)getPushToken;

@end
