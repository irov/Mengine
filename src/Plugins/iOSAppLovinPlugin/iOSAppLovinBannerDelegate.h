#pragma once

#import "Environment/Apple/AppleIncluder.h"

#import "iOSAppLovinBaseDelegate.h"

#import "Configuration/Configurations.h"

#if defined(MENGINE_PLUGIN_IOS_APPLOVIN_MEDIATION_AMAZON)
#import "iOSAppLovinBannerAmazonLoader.h"
#endif

#import <AppLovinSDK/AppLovinSDK.h>

@interface iOSAppLovinBannerDelegate : iOSAppLovinBaseDelegate<MAAdRequestDelegate, MAAdViewAdDelegate, MAAdRevenueDelegate, MAAdReviewDelegate>

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

@property (nonatomic, strong) MAAdView * _Nullable m_adView;

@property (assign) BOOL m_bannerAdaptive;

#if defined(MENGINE_PLUGIN_IOS_APPLOVIN_MEDIATION_AMAZON)
@property (nonatomic, strong) iOSAppLovinBannerAmazonLoader * _Nullable m_amazonLoader;
#endif

@end
