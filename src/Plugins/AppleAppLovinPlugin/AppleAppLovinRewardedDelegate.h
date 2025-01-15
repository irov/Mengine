#pragma once

#import "AppleAppLovinBaseDelegate.h"

#include "Configuration/Configurations.h"

#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
#import "AppleAppLovinRewardedAmazonLoader.h"
#endif

#import <AppLovinSDK/AppLovinSDK.h>
#import <Foundation/Foundation.h>

typedef void (^AppleAppLovinRewardedShowCompletionHandler)(BOOL success, NSDictionary<NSString *, id> * _Nonnull params);

@interface AppleAppLovinRewardedDelegate : AppleAppLovinBaseDelegate<MAAdRequestDelegate, MARewardedAdDelegate, MAAdRevenueDelegate, MAAdReviewDelegate>

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull)adUnitId;

- (BOOL) canOffer:(NSString * _Nonnull)placement;
- (BOOL) canYouShow:(NSString * _Nonnull)placement;
- (BOOL) show:(NSString * _Nonnull)placement completion:(AppleAppLovinRewardedShowCompletionHandler _Nonnull)completion;

- (void) loadAd;

@property (nonatomic, strong) MARewardedAd * _Nullable m_rewardedAd;
@property (nonatomic, strong) AppleAppLovinRewardedShowCompletionHandler _Nullable m_showCompletion;

#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
@property (nonatomic, strong) AppleAppLovinRewardedAmazonLoader * _Nullable m_amazonLoader;
#endif

@end
