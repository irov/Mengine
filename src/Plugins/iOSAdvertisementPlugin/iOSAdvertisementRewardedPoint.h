#pragma once

#import "Environment/Apple/AppleIncluder.h"

#import "iOSAdvertisementBasePoint.h"

@interface iOSAdvertisementRewardedPoint : iOSAdvertisementBasePoint

- (BOOL)canOfferAd;
- (BOOL)canYouShowAd;

@end
