#pragma once

#import "AppleAppLovinBaseDelegate.h"

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>

#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
#import "AppleAppLovinBannerAmazonLoader.h"
#endif

@interface AppleAppLovinBannerDelegate : AppleAppLovinBaseDelegate<MAAdRequestDelegate, MAAdViewAdDelegate, MAAdRevenueDelegate>

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                         placement:(NSString * _Nonnull) placement
                                      amazonSlotId:(NSString * _Nullable) amazonSlotId
                                              rect:(CGRect) rect
                                           provider:(const Mengine::AppleAppLovinBannerProviderInterfacePtr &) provider
                                         analytics:(AppleAppLovinAnalyticsService * _Nonnull) analytics;

- (void) show;
- (void) hide;

- (void) loadAd;

@property (nonatomic, strong) MAAdView * _Nullable m_adView;

@property (assign) Mengine::AppleAppLovinBannerProviderInterfacePtr m_provider;

#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
@property (nonatomic, strong) AppleAppLovinBannerAmazonLoader * _Nullable m_amazonLoader;
#endif

@end
