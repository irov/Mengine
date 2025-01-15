#pragma once

#import "AppleAppLovinBaseDelegate.h"

#include "Configuration/Configurations.h"

#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
#import "AppleAppLovinInterstitialAmazonLoader.h"
#endif

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>


typedef void (^AppleAppLovinInterstitialShowCompletionHandler)(BOOL success, NSDictionary<NSString *, id> * _Nonnull params);

@interface AppleAppLovinInterstitialDelegate : AppleAppLovinBaseDelegate<MAAdRequestDelegate, MAAdDelegate, MAAdRevenueDelegate, MAAdReviewDelegate>

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId;

- (BOOL) canYouShow:(NSString * _Nonnull) placement;
- (BOOL) show:(NSString * _Nonnull) placement completion:(AppleAppLovinInterstitialShowCompletionHandler _Nonnull)completion;

- (void) loadAd;

@property (nonatomic, strong) MAInterstitialAd * _Nullable m_interstitialAd;
@property (nonatomic, strong) AppleAppLovinInterstitialShowCompletionHandler _Nullable m_showCompletion;

#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
@property (nonatomic, strong) AppleAppLovinInterstitialAmazonLoader * _Nullable m_amazonLoader;
#endif

@end
