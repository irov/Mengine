#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol AppleMARSDKProviderInterface <NSObject>

- (void)onPlatformInit:(NSDictionary *)params;
- (void)onRealName:(NSDictionary *)params;
- (void)onEventWithCode:(NSInteger)code msg:(NSString *)msg;
- (void)onEventCustom:(NSString *)eventName params:(NSDictionary *)params;

- (void)onUserLogin:(NSDictionary *)params;
- (void)onUserLogout:(NSDictionary *)params;
- (void)onPayPaid:(NSDictionary *)params;

- (void)onPropComplete:(NSString *)orderId;
- (void)onPropError:(NSString *)orderId;

- (void)onPurchasedNonConsumable:(NSArray<NSString *> *)purchased;

- (void)onAdRewardedDidFailed;
- (void)onAdRewardedDidLoaded;
- (void)onAdRewardedDidShow;
- (void)onAdRewardedDidShowFailed;
- (void)onAdRewardedDidClicked;
- (void)onAdRewardedDidClosed;
- (void)onAdRewardedDidSkipped;
- (void)onAdRewardedDidFinished:(NSString *)itemName itemNum:(NSUInteger)itemNum;

@end

@protocol AppleMARSDKInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)setProvider:(id<AppleMARSDKProviderInterface>)provider;
- (id<AppleMARSDKProviderInterface>)getProvider;

- (BOOL)login;
- (BOOL)logout;
- (BOOL)switchAccount;

- (void)requestNonConsumablePurchased;

- (void)submitExtendedData:(NSString *)data;
- (void)submitPaymentData:(NSString *)data;

- (void)propComplete:(NSString *)productId;

- (void)showRewardVideoAd:(NSString *)itemName itemNum:(NSUInteger)itemNum;

- (NSInteger)getInternetDate;

@end
