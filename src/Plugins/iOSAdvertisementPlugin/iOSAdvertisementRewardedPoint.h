#pragma once

#import "Environment/Apple/AppleIncluder.h"

#import "iOSAdvertisementBasePoint.h"

@class iOSAdvertisementFrequency;

@interface iOSAdvertisementRewardedPoint : iOSAdvertisementBasePoint

- (void)setFrequency:(iOSAdvertisementFrequency *)frequency;

- (BOOL)canOfferAd;
- (BOOL)canYouShowAd;

- (NSInteger)getShowLimit;
- (NSInteger)getShowTimeout;
- (BOOL)hasShowQuota;
- (void)recordShown;

@property (nonatomic) NSInteger m_showLimit;
@property (nonatomic) NSInteger m_showTimeout;

@end
