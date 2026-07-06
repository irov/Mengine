#pragma once

#include "iOSAppLovinInterface.h"

#import <Foundation/Foundation.h>

@interface iOSAppLovinAmazonService : NSObject

- (instancetype _Nonnull) init;
- (NSString * _Nullable) getAmazonBannerSlotId;
- (NSString * _Nullable) getAmazonInterstitialSlotId;
- (NSString * _Nullable) getAmazonRewardedSlotId;

@end
