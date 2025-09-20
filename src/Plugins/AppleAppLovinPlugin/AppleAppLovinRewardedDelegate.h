#pragma once

#import "AppleAppLovinBaseDelegate.h"

#import "Environment/Apple/AppleIncluder.h"

#include "Configuration/Configurations.h"

#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
#import "AppleAppLovinRewardedAmazonLoader.h"
#endif

#import <AppLovinSDK/AppLovinSDK.h>

@interface AppleAppLovinRewardedDelegate : AppleAppLovinBaseDelegate<MAAdRequestDelegate, MARewardedAdDelegate, MAAdRevenueDelegate, MAAdExpirationDelegate, MAAdReviewDelegate>

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull)adUnitId advertisement:(id<AppleAdvertisementInterface> _Nonnull)advertisement;

- (BOOL) canOffer:(NSString * _Nonnull)placement;
- (BOOL) canYouShow:(NSString * _Nonnull)placement;
- (BOOL) show:(NSString * _Nonnull)placement;
- (BOOL) isShowing;

- (void) loadAd;

@property (nonatomic, strong) MARewardedAd * _Nullable m_rewardedAd;

@property (atomic, assign) BOOL m_showing;

#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
@property (nonatomic, strong) AppleAppLovinRewardedAmazonLoader * _Nullable m_amazonLoader;
#endif

@end
