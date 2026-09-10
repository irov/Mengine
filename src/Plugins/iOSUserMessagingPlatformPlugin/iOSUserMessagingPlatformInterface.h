#pragma once

#import "Environment/Apple/AppleIncluder.h"

typedef NS_ENUM(NSUInteger, iOSUserMessagingPlatformConsentState) {
    iOSUserMessagingPlatformConsentStatePending = 0,
    iOSUserMessagingPlatformConsentStateCompleted = 1,
    iOSUserMessagingPlatformConsentStateFailed = 2
};

@protocol iOSUserMessagingPlatformInterface <NSObject>

+ (instancetype)sharedInstance;

- (BOOL)isComplete;
- (iOSUserMessagingPlatformConsentState)getConsentState;
- (void)retryConsent;

@end
