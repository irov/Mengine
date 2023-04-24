#pragma once

#import "AppleAppLovinBaseDelegate.h"

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>

#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
#import "AppleAppLovinBannerAmazonLoader.h"
#endif

@interface AppleAppLovinBannerDelegate : AppleAppLovinBaseDelegate<MAAdViewAdDelegate, MAAdRevenueDelegate>

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) AdUnitIdentifier
                                      amazonSlotId:(NSString * _Nullable) amazonSlotId
                                              rect:(CGRect) rect
                                  analyticsService:(AppleAppLovinAnalyticsService * _Nonnull) analytics;

- (void) show;
- (void) hide;

- (void) loadAd;

- (UIViewController* _Nullable) rootViewController;

@property (nonatomic, strong) MAAdView * _Nullable m_adView;

#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
@property (nonatomic, strong) AppleAppLovinBannerAmazonLoader * _Nullable m_amazonLoader;
#endif

@end
