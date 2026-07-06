#pragma once

#import "Environment/Apple/AppleIncluder.h"

#import "iOSAdMobBaseDelegate.h"

#include "Configuration/Configurations.h"

#import <GoogleMobileAds/GoogleMobileAds.h>

@interface iOSAdMobBannerDelegate : iOSAdMobBaseDelegate<GADBannerViewDelegate, GADAdSizeDelegate>

- (instancetype _Nullable)initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                     advertisement:(id<iOSAdvertisementInterface> _Nonnull)advertisement
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
@property (nonatomic, strong) NSString * _Nonnull m_placement;

@property (assign) BOOL m_bannerAdaptive;

@end
