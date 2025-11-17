#pragma once

#import "AppleAdMobBaseDelegate.h"

#import "Environment/Apple/AppleIncluder.h"

#include "Configuration/Configurations.h"

#import <GoogleMobileAds/GoogleMobileAds.h>

@interface AppleAdMobInterstitialDelegate : AppleAdMobBaseDelegate<GADFullScreenContentDelegate>

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId advertisement:(id<AppleAdvertisementInterface> _Nonnull)advertisement;

- (BOOL) canYouShow:(NSString * _Nonnull) placement;
- (BOOL) show:(NSString * _Nonnull) placement;
- (BOOL) isShowing;

- (void) loadAd;

@property (nonatomic, strong) GADInterstitialAd * _Nullable m_interstitialAd;

@property (atomic, assign) BOOL m_showing;

@end

