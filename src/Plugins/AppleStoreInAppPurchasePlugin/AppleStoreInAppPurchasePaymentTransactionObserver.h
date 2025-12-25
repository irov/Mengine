#pragma once

#include "AppleStoreInAppPurchaseInterface.h"
#include "AppleStoreInAppPurchaseFactoryInterface.h"

#import "Environment/Apple/AppleIncluder.h"

#import <StoreKit/StoreKit.h>

@interface AppleStoreInAppPurchasePaymentTransactionObserver : NSObject <SKPaymentTransactionObserver>

@property (assign) Mengine::AppleStoreInAppPurchaseFactoryInterface * _Nullable m_factory;
@property (assign) Mengine::AppleStoreInAppPurchaseServiceInterface * _Nullable m_service;

@property (strong) NSSet<NSString *> * _Nullable m_consumableIdentifiers;
@property (strong) NSSet<NSString *> * _Nullable m_nonconsumableIdentifiers;

@property (strong) NSMutableArray<NSDictionary *> * _Nonnull m_cacheSKPaymentTransactions;

+ (instancetype _Nonnull) sharedInstance;

- (instancetype _Nonnull) init;

- (void)activateWithFactory:(Mengine::AppleStoreInAppPurchaseFactoryInterface * _Nonnull)_factory service:(Mengine::AppleStoreInAppPurchaseServiceInterface * _Nonnull)_service consumableIdentifiers:(NSSet * _Nonnull)_consumableIdentifiers nonconsumableIdentifiers:(NSSet * _Nonnull)_nonconsumableIdentifiers;

- (void)deactivate;

@end
