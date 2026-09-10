#import "iOSAdvertisementRewardedPoint.h"

@implementation iOSAdvertisementRewardedPoint

- (BOOL)canOfferAd {
    if ([self isEnabled] == NO) {
        return NO;
    }

    return YES;
}

- (BOOL)canYouShowAd {
    if ([self isEnabled] == NO) {
        return NO;
    }

    [self.m_attempts incrementAttempts];

    return YES;
}

@end
