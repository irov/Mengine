#pragma once

#import "AppleAppLovinBaseDelegate.h"

#include "Configuration/Configurations.h"

#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
#import "AppleAppLovinBannerAmazonLoader.h"
#endif

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>

@interface AppleAppLovinBannerDelegate : AppleAppLovinBaseDelegate<MAAdRequestDelegate, MAAdViewAdDelegate, MAAdRevenueDelegate, MAAdReviewDelegate>

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                          placement:(NSString * _Nonnull) placement
                                       amazonSlotId:(NSString * _Nullable) amazonSlotId
                                               rect:(CGRect) rect
                                           provider:(const Mengine::AppleAppLovinBannerProviderInterfacePtr &) provider;

- (void) show;
- (void) hide;

- (void) loadAd;

- (CGRect) getRect;

@property (nonatomic, strong) MAAdView * _Nullable m_adView;

@property (assign) Mengine::AppleAppLovinBannerProviderInterfacePtr m_provider;

#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
@property (nonatomic, strong) AppleAppLovinBannerAmazonLoader * _Nullable m_amazonLoader;
#endif

@end
