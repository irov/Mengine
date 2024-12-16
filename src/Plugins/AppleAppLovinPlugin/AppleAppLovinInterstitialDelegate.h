#pragma once

#import "AppleAppLovinBaseDelegate.h"

#include "Configuration/Configurations.h"

#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
#import "AppleAppLovinInterstitialAmazonLoader.h"
#endif

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>

@interface AppleAppLovinInterstitialDelegate : AppleAppLovinBaseDelegate<MAAdRequestDelegate, MAAdDelegate, MAAdRevenueDelegate, MAAdReviewDelegate>

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                       amazonSlotId:(NSString * _Nullable) amazonSlotId
                                           provider:(const Mengine::AppleAppLovinInterstitialProviderInterfacePtr &) provider;

- (BOOL) canYouShow:(NSString * _Nonnull) placement;
- (BOOL) show:(NSString * _Nonnull) placement;

- (void) loadAd;

@property (assign) Mengine::AppleAppLovinInterstitialProviderInterfacePtr m_provider;

@property (nonatomic, strong) MAInterstitialAd * _Nullable m_interstitialAd;

#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
@property (nonatomic, strong) AppleAppLovinInterstitialAmazonLoader * _Nullable m_amazonLoader;
#endif

@end
