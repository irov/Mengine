#pragma once

#include "AppleAppLovinInterface.h"

#import "AppleAppLovinAnalyticsService.h"

#import <AppLovinSDK/AppLovinSDK.h>
#import <Foundation/Foundation.h>

#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
#import "AppleAppLovinRewardedAmazonLoader.h"
#endif

@interface AppleAppLovinRewardedDelegate : NSObject<MARewardedAdDelegate, MAAdRevenueDelegate>

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitIdentifier
                                      amazonSlotId:(NSString * _Nullable) amazonSlotId
                                  analyticsService:(AppleAppLovinAnalyticsService * _Nonnull) analytics;

- (BOOL) canOffer;
- (BOOL) canYouShow;
- (BOOL) show;

- (void) loadAd;

@property (nonatomic, assign) AppleAppLovinAnalyticsService * _Nonnull m_analytics;
@property (nonatomic, strong) MARewardedAd * _Nullable m_rewardedAd;
@property (nonatomic, assign) NSInteger m_retryAttempt;
@property (nonatomic, assign) NSInteger m_enumeratorRequest;
@property (nonatomic, assign) NSInteger m_requestId;

#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
@property (nonatomic, assign) AppleAppLovinRewardedAmazonLoader * _Nullable m_amazonLoader;
#endif

@end
