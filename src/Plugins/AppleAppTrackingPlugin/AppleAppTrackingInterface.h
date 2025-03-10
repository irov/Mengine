#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, EAppleAppTrackingAuthorization) {
    EAATA_AUTHORIZED,
    EAATA_DENIED,
    EAATA_RESTRICTED,
    EAATA_NOT_DETERMINED
};

@protocol AppleAppTrackingInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)authorization:(void (^)(EAppleAppTrackingAuthorization status, NSString *idfa))response;
- (EAppleAppTrackingAuthorization)getAuthorizationStatus;
- (NSString *)getIDFA;
- (BOOL)isTrackingAllowed;

@end
