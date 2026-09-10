#pragma once

#import "Environment/iOS/iOSAppTrackingAuthorization.h"

typedef NS_ENUM(NSUInteger, iOSAppTrackingAuthorizationState) {
    iOSAppTrackingAuthorizationStatePending = 0,
    iOSAppTrackingAuthorizationStateCompleted = 1,
    iOSAppTrackingAuthorizationStateFailed = 2
};

@protocol iOSAppTrackingInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)authorization;

- (EiOSAppTrackingAuthorization)getAuthorizationStatus;
- (iOSAppTrackingAuthorizationState)getAuthorizationState;
- (NSString *)getIDFA;
- (BOOL)isTrackingAllowed;

@end
