#pragma once

#import "AppleAppLovinBaseDelegate.h"

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>

#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
#import "AppleAppLovinInterstitialAmazonLoader.h"
#endif

@interface AppleAppLovinInterstitialDelegate : AppleAppLovinBaseDelegate<MAAdRequestDelegate, MAAdDelegate, MAAdRevenueDelegate>

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                      amazonSlotId:(NSString * _Nullable) amazonSlotId
                                          provider:(const Mengine::AppleAppLovinInterstitialProviderInterfacePtr &) provider
                                         analytics:(AppleAppLovinAnalyticsService * _Nonnull) analytics;

- (BOOL) canYouShow:(NSString * _Nonnull) placement;
- (BOOL) show:(NSString * _Nonnull) placement;

- (void) loadAd;

@property (assign) Mengine::AppleAppLovinInterstitialProviderInterfacePtr m_provider;

@property (nonatomic, strong) MAInterstitialAd * _Nullable m_interstitialAd;

#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
@property (nonatomic, strong) AppleAppLovinInterstitialAmazonLoader * _Nullable m_amazonLoader;
#endif

@end
