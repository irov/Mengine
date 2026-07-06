#pragma once

#import "iOSAdMobBaseDelegate.h"

#import "Environment/Apple/AppleIncluder.h"

#include "Configuration/Configurations.h"

#import <GoogleMobileAds/GoogleMobileAds.h>

@interface iOSAdMobInterstitialDelegate : iOSAdMobBaseDelegate<GADFullScreenContentDelegate>

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId advertisement:(id<iOSAdvertisementInterface> _Nonnull)advertisement;

- (BOOL) canYouShow:(NSString * _Nonnull) placement;
- (BOOL) show:(NSString * _Nonnull) placement;
- (BOOL) isShowing;

- (void) loadAd;

@property (nonatomic, strong) GADInterstitialAd * _Nullable m_interstitialAd;
@property (nonatomic, strong) NSString * _Nullable m_placement;

@property (atomic, assign) BOOL m_showing;

@end
