#pragma once

#import "Environment/Apple/AppleIncluder.h"

#import "AppleAdvertisementBasePoint.h"

@interface AppleAdvertisementRewardedPoint : AppleAdvertisementBasePoint

- (instancetype)initWithName:(NSString *)name withJson:(NSDictionary *)json;

- (BOOL)canOfferAd;
- (BOOL)canYouShowAd;

@end
