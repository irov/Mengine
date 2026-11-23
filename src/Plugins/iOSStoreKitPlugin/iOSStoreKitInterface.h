#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol iOSStoreKitProviderInterface <NSObject>

- (void)onStoreKitProductsResponse:(NSArray<NSDictionary *> *)products;
- (void)onStoreKitProductsFailed;
- (void)onStoreKitTransaction:(NSDictionary *)transaction;
- (void)onStoreKitTransactionFinished:(NSString *)productIdentifier transactionIdentifier:(NSString *)transactionIdentifier;
- (void)onStoreKitRestoreResult:(NSArray<NSDictionary *> *)transactions successful:(BOOL)successful;
- (void)onStoreKitPurchaseCancelled:(NSString *)productIdentifier;
- (void)onStoreKitPurchasePending:(NSString *)productIdentifier;
- (void)onStoreKitPurchaseFailed:(NSString *)productIdentifier;
- (void)onStoreKitEntitlementChanged:(NSString *)productIdentifier owned:(BOOL)owned;
- (void)onStoreKitSubscriptionStatus:(NSString *)productIdentifier status:(NSDictionary *)status;

@end

@protocol iOSStoreKitInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)setProvider:(id<iOSStoreKitProviderInterface>)provider;
- (id<iOSStoreKitProviderInterface>)getProvider;

- (BOOL)canMakePayments;
- (void)requestProducts:(NSArray<NSString *> *)consumableIdentifiers nonConsumableIdentifiers:(NSArray<NSString *> *)nonConsumableIdentifiers;
- (BOOL)purchaseProduct:(NSString *)productIdentifier accountToken:(NSString *)accountToken;
- (void)completeTransactionDelivery:(NSString *)transactionIdentifier successful:(BOOL)successful;
- (void)restorePurchases;
- (BOOL)isOwnedProduct:(NSString *)productIdentifier;
- (void)querySubscriptionStatus:(NSString *)productIdentifier;
- (BOOL)showManageSubscriptions;

@end
