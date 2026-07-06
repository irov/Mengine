#pragma once

#import "Environment/iOS/iOSAppTrackingAuthorization.h"

@protocol iOSAppTrackingInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)authorization;

- (EiOSAppTrackingAuthorization)getAuthorizationStatus;
- (NSString *)getIDFA;
- (BOOL)isTrackingAllowed;

@end
