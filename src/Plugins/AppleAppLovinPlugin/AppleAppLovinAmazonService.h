#pragma once

#include "AppleAppLovinInterface.h"

#import <Foundation/Foundation.h>

@interface AppleAppLovinAmazonService : NSObject

- (instancetype _Nonnull) init;
- (NSString * _Nullable) getAmazonBannerSlotId;
- (NSString * _Nullable) getAmazonInterstitialSlotId;
- (NSString * _Nullable) getAmazonRewardedSlotId;

@end
