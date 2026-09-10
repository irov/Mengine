#pragma once

#import "Environment/Apple/AppleIncluder.h"
#import "Environment/iOS/iOSAdFormat.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM( NSInteger, EiOSAdvertisementBannerAnchor )
{
    IOS_ADVERTISEMENT_BANNER_ANCHOR_BOTTOM,
    IOS_ADVERTISEMENT_BANNER_ANCHOR_TOP
};

@protocol iOSAdvertisementCallbackInterface <NSObject>
- (void)oniOSAdvertisementShowSuccess:(NSString *)placement;
- (void)oniOSAdvertisementShowFailed:(NSString *)placement withError:(NSInteger)code;
- (void)oniOSAdvertisementRevenuePaid:(NSString *)placement withRevenue:(double)revenue;
- (void)oniOSAdvertisementUserRewarded:(NSString *)placement withLabel:(NSString *)label withAmount:(NSInteger)amount;
@end

@protocol iOSAdvertisementProviderInterface <NSObject>
- (BOOL)hasBanner;
- (BOOL)isBannerLoaded;
- (void)showBanner;
- (void)hideBanner;
- (BOOL)getBannerWidth:(uint32_t *)width height:(uint32_t *)height;

- (BOOL)hasTopper;
- (BOOL)isTopperLoaded;
- (void)showTopper;
- (void)hideTopper;
- (BOOL)getTopperWidth:(uint32_t *)width height:(uint32_t *)height;

- (BOOL)hasInterstitial;
- (BOOL)canYouShowInterstitial:(NSString *)placement;
- (BOOL)showInterstitial:(NSString *)placement;
- (BOOL)isShowingInterstitial;

- (BOOL)hasRewarded;
- (BOOL)canOfferRewarded:(NSString *)placement;
- (BOOL)canYouShowRewarded:(NSString *)placement;
- (BOOL)showRewarded:(NSString *)placement;
- (BOOL)isShowingRewarded;

- (BOOL)hasRewardedInterstitial;
- (BOOL)canYouShowRewardedInterstitial:(NSString *)placement;
- (BOOL)showRewardedInterstitial:(NSString *)placement;
- (BOOL)isShowingRewardedInterstitial;
@end

@protocol iOSAdvertisementInterface <iOSAdvertisementProviderInterface>
+ (instancetype)sharedInstance;

- (void)setProvider:(id<iOSAdvertisementProviderInterface> _Nullable)provider;
- (id<iOSAdvertisementProviderInterface> _Nullable)getProvider;

- (void)readyAdProvider;

- (BOOL)isShowingInterstitial;
- (BOOL)isShowingRewarded;

- (BOOL)getNoAds;

- (void)setBannerCallback:(id<iOSAdvertisementCallbackInterface> _Nullable)callback;
- (id<iOSAdvertisementCallbackInterface> _Nullable)getBannerCallback;

- (void)setInterstitialCallback:(id<iOSAdvertisementCallbackInterface> _Nullable)callback;
- (id<iOSAdvertisementCallbackInterface> _Nullable)getInterstitialCallback;

- (void)setRewardedCallback:(id<iOSAdvertisementCallbackInterface> _Nullable)callback;
- (id<iOSAdvertisementCallbackInterface> _Nullable)getRewardedCallback;

- (void)setAdFreeze:(NSString *)adName freeze:(BOOL)freeze;
@end

NS_ASSUME_NONNULL_END
