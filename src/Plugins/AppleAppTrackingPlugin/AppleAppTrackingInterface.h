#pragma once

#import "Environment/iOS/iOSAppTrackingAuthorization.h"

@protocol AppleAppTrackingInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)authorization;

- (EAppleAppTrackingAuthorization)getAuthorizationStatus;
- (NSString *)getIDFA;
- (BOOL)isTrackingAllowed;

@end
