#pragma once

#import "Environment/Apple/AppleIncluder.h"

#import "iOSAdvertisementBasePoint.h"

@interface iOSAdvertisementRewardedPoint : iOSAdvertisementBasePoint

- (BOOL)canOfferAd;
- (BOOL)canYouShowAd;

- (NSInteger)getShowLimit;
- (NSInteger)getShowTimeout;
- (BOOL)hasShowQuota;
- (void)recordShown;

@property (nonatomic) NSInteger m_showLimit;
@property (nonatomic) NSInteger m_showTimeout;
@property (nonatomic, copy) NSString * m_showStartKey;
@property (nonatomic, copy) NSString * m_showCountKey;

@end
