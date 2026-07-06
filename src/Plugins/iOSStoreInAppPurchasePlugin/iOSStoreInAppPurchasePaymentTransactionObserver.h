#pragma once

#include "iOSStoreInAppPurchaseInterface.h"

#import "Environment/Apple/AppleIncluder.h"

#import <StoreKit/StoreKit.h>

@interface iOSStoreInAppPurchasePaymentTransactionObserver : NSObject <SKPaymentTransactionObserver>

@property (nonatomic, weak) id<iOSStoreInAppPurchaseInterface> _Nullable m_inAppPurchase;

@property (strong) NSSet<NSString *> * _Nullable m_consumableIdentifiers;
@property (strong) NSSet<NSString *> * _Nullable m_nonconsumableIdentifiers;

@property (strong) NSMutableArray<NSDictionary *> * _Nonnull m_cacheSKPaymentTransactions;

+ (instancetype _Nonnull) sharedInstance;

- (instancetype _Nonnull) init;

- (void)activateWithInAppPurchase:(id<iOSStoreInAppPurchaseInterface> _Nonnull)inAppPurchase
            consumableIdentifiers:(NSSet<NSString *> * _Nonnull)consumableIdentifiers
         nonconsumableIdentifiers:(NSSet<NSString *> * _Nonnull)nonconsumableIdentifiers;

- (void)deactivate;

@end
