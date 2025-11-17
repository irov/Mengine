#pragma once

#import "Environment/Apple/AppleIncluder.h"

#import "AppleAdMobBaseDelegate.h"

#include "Configuration/Configurations.h"

#import <GoogleMobileAds/GoogleMobileAds.h>

@interface AppleAdMobBannerDelegate : AppleAdMobBaseDelegate<GADBannerViewDelegate>

- (instancetype _Nullable)initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                     advertisement:(id<AppleAdvertisementInterface> _Nonnull)advertisement
                                         placement:(NSString * _Nonnull) placement
                                          adaptive:(BOOL) adaptive;

- (void)show;
- (void)hide;

- (void)loadAd;

- (CGSize)getSize;
- (CGRect)getRect;

- (CGFloat)getHeightPx;
- (CGFloat)getWidthPx;

@property (nonatomic, strong) GADBannerView * _Nullable m_bannerView;

@property (assign) BOOL m_bannerAdaptive;

@end

