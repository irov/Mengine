#pragma once

#import "Environment/Apple/AppleIncluder.h"

#import "iOSAdvertisementBasePoint.h"

@interface iOSAdvertisementRewardedPoint : iOSAdvertisementBasePoint

- (instancetype)initWithName:(NSString *)name withJson:(NSDictionary *)json;

- (BOOL)canOfferAd;
- (BOOL)canYouShowAd;

@end
