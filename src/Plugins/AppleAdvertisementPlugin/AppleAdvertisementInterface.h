#pragma once

#import "Environment/iOS/iOSAdFormat.h"

#import <Foundation/Foundation.h>

@protocol AppleAdvertisementCallbackInterface <NSObject>
- (void)onAppleAdvertisementShowSuccess:(NSString *)placement;
- (void)onAppleAdvertisementShowFailed:(NSString *)placement withError:(NSInteger)code;
- (void)onAppleAdvertisementRevenuePaid:(NSString *)placement withRevenue:(double)revenue;
- (void)onAppleAdvertisementUserRewarded:(NSString *)placement withLabel:(NSString *)label withAmount:(NSInteger)amount;
@end

@protocol AppleAdvertisementProviderInterface <NSObject>
- (BOOL)hasBanner;
- (BOOL)showBanner;
- (BOOL)hideBanner;
- (BOOL)getBannerWidth:(uint32_t *)width height:(uint32_t *)height;

- (BOOL)hasInterstitial;
- (BOOL)canYouShowInterstitial:(NSString *)placement;
- (BOOL)showInterstitial:(NSString *)placement;

- (BOOL)hasRewarded;
- (BOOL)canOfferRewarded:(NSString *)placement;
- (BOOL)canYouShowRewarded:(NSString *)placement;
- (BOOL)showRewarded:(NSString *)placement;
@end

@protocol AppleAdvertisementInterface <AppleAdvertisementProviderInterface>
+ (instancetype)sharedInstance;

- (void)setProvider:(id<AppleAdvertisementProviderInterface>)provider;
- (id<AppleAdvertisementProviderInterface>)getProvider;

- (void)setBannerCallback:(id<AppleAdvertisementCallbackInterface>)callback;
- (id<AppleAdvertisementCallbackInterface>)getBannerCallback;

- (void)setInterstitialCallback:(id<AppleAdvertisementCallbackInterface>)callback;
- (id<AppleAdvertisementCallbackInterface>)getInterstitialCallback;

- (void)setRewardedCallback:(id<AppleAdvertisementCallbackInterface>)callback;
- (id<AppleAdvertisementCallbackInterface>)getRewardedCallback;
@end
