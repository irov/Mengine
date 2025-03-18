#pragma once

#include "Config/Config.h"

#import "AppleAdvertisementBasePoint.h"

#import <Foundation/Foundation.h>

@interface AppleAdvertisementRewardedPoint : AppleAdvertisementBasePoint

- (instancetype)initWithName:(NSString *)name withJson:(NSDictionary *)json;

- (BOOL)canOfferAd;
- (BOOL)canYouShowAd;

@end
