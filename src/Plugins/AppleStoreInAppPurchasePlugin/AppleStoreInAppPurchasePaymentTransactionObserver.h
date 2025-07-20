#pragma once

#include "AppleStoreInAppPurchaseInterface.h"
#include "AppleStoreInAppPurchaseFactoryInterface.h"

#import "Environment/Apple/AppleIncluder.h"

#import <StoreKit/StoreKit.h>

@interface AppleStoreInAppPurchasePaymentTransactionObserver : NSObject <SKPaymentTransactionObserver>

@property (assign) Mengine::AppleStoreInAppPurchaseFactoryInterface * _Nullable m_factory;
@property (assign) Mengine::AppleStoreInAppPurchaseServiceInterface * _Nullable m_service;

@property (strong) NSMutableArray<NSDictionary *> * _Nonnull m_cacheSKPaymentTransactions;

+ (instancetype _Nonnull) sharedInstance;

- (instancetype _Nonnull) init;

- (void)activateWithFactory: (Mengine::AppleStoreInAppPurchaseFactoryInterface * _Nonnull)_factory service: (Mengine::AppleStoreInAppPurchaseServiceInterface * _Nonnull)_service;

- (void)deactivate;

@end
