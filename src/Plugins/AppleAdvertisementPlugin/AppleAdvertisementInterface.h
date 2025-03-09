#pragma once

#import "Environment/iOS/iOSAdFormat.h"

@protocol AppleAdvertisementResponseInterface <NSObject>
- (void)onAppleAdvertisementInterstitialShowSuccessful:(iOSAdFormat *)format withPlacement:(NSString *)placement;
- (void)onAppleAdvertisementInterstitialShowFailed:(iOSAdFormat *)format withPlacement:(NSString *)placement withError:(int)code;
- (void)onAppleAdvertisementInterstitialRevenuePaid:(iOSAdFormat *)format withPlacement:(NSString *)placement withRevenue:(double)revenue;
- (void)onAppleAdvertisementRewardedUserRewarded:(iOSAdFormat *)format withPlacement:(NSString *)placement withLabel:(NSString *)label withAmount:(int)amount;
@end

@protocol AppleAdvertisementProviderInterface <NSObject>
- (BOOL)hasBanner;
- (BOOL)showBanner;
- (BOOL)hideBanner;
- (BOOL)getBannerSize:(uint32_t *)width height:(uint32_t *)height;

- (BOOL)hasInterstitial;
- (BOOL)canYouShowInterstitial:(NSString *)placement;
- (BOOL)showInterstitial:(NSString *)placement;

- (BOOL)hasRewarded;
- (BOOL)canOfferRewarded:(NSString *)placement;
- (BOOL)canYouShowRewarded:(NSString *)placement;
- (BOOL)showRewarded:(NSString *)placement;
@end

@protocol AppleAdvertisementInterface <AppleAdvertisementProviderInterface>
+ (id<AppleAdvertisementInterface>)sharedInstance;

- (void)setAdvertisementProvider:(id<AppleAdvertisementProviderInterface>)provider;
- (id<AppleAdvertisementProviderInterface>)getAdvertisementProvider;
@end
