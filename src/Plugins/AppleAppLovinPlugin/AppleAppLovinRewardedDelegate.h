#pragma once

#import "AppleAppLovinBaseDelegate.h"

#import <AppLovinSDK/AppLovinSDK.h>
#import <Foundation/Foundation.h>

#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
#import "AppleAppLovinRewardedAmazonLoader.h"
#endif

@interface AppleAppLovinRewardedDelegate : AppleAppLovinBaseDelegate<MAAdRequestDelegate, MARewardedAdDelegate, MAAdRevenueDelegate, MAAdReviewDelegate>

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                       amazonSlotId:(NSString * _Nullable) amazonSlotId
                                           provider:(const Mengine::AppleAppLovinRewardedProviderInterfacePtr &) provider
                                          analytics:(AppleAppLovinAnalyticsService * _Nonnull) analytics;

- (BOOL) canOffer:(NSString * _Nonnull) placement;
- (BOOL) canYouShow:(NSString * _Nonnull) placement;
- (BOOL) show:(NSString * _Nonnull) placement;

- (void) loadAd;

@property (assign) Mengine::AppleAppLovinRewardedProviderInterfacePtr m_provider;

@property (nonatomic, strong) MARewardedAd * _Nullable m_rewardedAd;

#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
@property (nonatomic, strong) AppleAppLovinRewardedAmazonLoader * _Nullable m_amazonLoader;
#endif

@end
