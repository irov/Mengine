#pragma once

#import "Environment/Apple/AppleIncluder.h"
#import "Environment/iOS/iOSAdFormat.h"

@protocol iOSAdvertisementCallbackInterface <NSObject>
- (void)oniOSAdvertisementShowSuccess:(NSString *)placement;
- (void)oniOSAdvertisementShowFailed:(NSString *)placement withError:(NSInteger)code;
- (void)oniOSAdvertisementRevenuePaid:(NSString *)placement withRevenue:(double)revenue;
- (void)oniOSAdvertisementUserRewarded:(NSString *)placement withLabel:(NSString *)label withAmount:(NSInteger)amount;
@end

@protocol iOSAdvertisementProviderInterface <NSObject>
- (BOOL)hasBanner;
- (BOOL)showBanner;
- (BOOL)hideBanner;
- (BOOL)getBannerWidth:(uint32_t *)width height:(uint32_t *)height;

- (BOOL)hasInterstitial;
- (BOOL)canYouShowInterstitial:(NSString *)placement;
- (BOOL)showInterstitial:(NSString *)placement;
- (BOOL)isShowingInterstitial;

- (BOOL)hasRewarded;
- (BOOL)canOfferRewarded:(NSString *)placement;
- (BOOL)canYouShowRewarded:(NSString *)placement;
- (BOOL)showRewarded:(NSString *)placement;
- (BOOL)isShowingRewarded;
@end

@protocol iOSAdvertisementInterface <iOSAdvertisementProviderInterface>
+ (instancetype)sharedInstance;

- (void)setProvider:(id<iOSAdvertisementProviderInterface>)provider;
- (id<iOSAdvertisementProviderInterface>)getProvider;

- (void)readyAdProvider;

- (BOOL)isShowingInterstitial;
- (BOOL)isShowingRewarded;

- (BOOL)getNoAds;

- (void)setBannerCallback:(id<iOSAdvertisementCallbackInterface>)callback;
- (id<iOSAdvertisementCallbackInterface>)getBannerCallback;

- (void)setInterstitialCallback:(id<iOSAdvertisementCallbackInterface>)callback;
- (id<iOSAdvertisementCallbackInterface>)getInterstitialCallback;

- (void)setRewardedCallback:(id<iOSAdvertisementCallbackInterface>)callback;
- (id<iOSAdvertisementCallbackInterface>)getRewardedCallback;

- (void)setAdFreeze:(NSString *)adName freeze:(BOOL)freeze;
@end
