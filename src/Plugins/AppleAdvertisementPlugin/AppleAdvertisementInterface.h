#pragma once

#import "Environment/iOS/iOSAdFormat.h"

#import <Foundation/Foundation.h>

@protocol AppleAdvertisementCallbackInterface <NSObject>
- (void)onAppleAdvertisementShowSuccess:(iOSAdFormat *)format withPlacement:(NSString *)placement;
- (void)onAppleAdvertisementShowFailed:(iOSAdFormat *)format withPlacement:(NSString *)placement withError:(NSInteger)code;
- (void)onAppleAdvertisementRevenuePaid:(iOSAdFormat *)format withPlacement:(NSString *)placement withRevenue:(double)revenue;
- (void)onAppleAdvertisementUserRewarded:(iOSAdFormat *)format withPlacement:(NSString *)placement withLabel:(NSString *)label withAmount:(NSInteger)amount;
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

- (void)setAdvertisementProvider:(id<AppleAdvertisementProviderInterface>)provider;
- (id<AppleAdvertisementProviderInterface>)getAdvertisementProvider;

- (void)setAdvertisementCallback:(id<AppleAdvertisementCallbackInterface>)callback;
- (id<AppleAdvertisementCallbackInterface>)getAdvertisementCallback;
@end
