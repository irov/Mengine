#pragma once

#import "iOSAppLovinBaseDelegate.h"

#import "Environment/Apple/AppleIncluder.h"

#include "Configuration/Configurations.h"

#if defined(MENGINE_PLUGIN_IOS_APPLOVIN_MEDIATION_AMAZON)
#import "iOSAppLovinInterstitialAmazonLoader.h"
#endif

#import <AppLovinSDK/AppLovinSDK.h>

@interface iOSAppLovinInterstitialDelegate : iOSAppLovinBaseDelegate<MAAdRequestDelegate, MAAdDelegate, MAAdRevenueDelegate, MAAdExpirationDelegate, MAAdReviewDelegate>

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId advertisement:(id<iOSAdvertisementInterface> _Nonnull)advertisement;

- (BOOL) canYouShow:(NSString * _Nonnull) placement;
- (BOOL) show:(NSString * _Nonnull) placement;
- (BOOL) isShowing;

- (void) loadAd;

@property (nonatomic, strong) MAInterstitialAd * _Nullable m_interstitialAd;

@property (atomic, assign) BOOL m_showing;

#if defined(MENGINE_PLUGIN_IOS_APPLOVIN_MEDIATION_AMAZON)
@property (nonatomic, strong) iOSAppLovinInterstitialAmazonLoader * _Nullable m_amazonLoader;
#endif

@end
