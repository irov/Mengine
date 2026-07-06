#pragma once

#import "Environment/Apple/AppleIncluder.h"

#import "iOSAdvertisementBasePoint.h"

@interface iOSAdvertisementInterstitialPoint : iOSAdvertisementBasePoint

- (instancetype)initWithName:(NSString *)name withJson:(NSDictionary *)json;

- (BOOL)canYouShowAd;

@property (nonatomic) NSInteger m_actionOffset;
@property (nonatomic) NSInteger m_actionCooldown;
@property (nonatomic) NSInteger m_timeOffset;
@property (nonatomic) NSInteger m_timeCooldown;
@property (nonatomic) NSInteger m_installTimeOffset;
@property (nonatomic) NSInteger m_sessionOffset;

@end
