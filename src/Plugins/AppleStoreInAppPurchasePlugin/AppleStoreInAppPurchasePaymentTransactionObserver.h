#include "AppleStoreInAppPurchaseInterface.h"
#include "AppleStoreInAppPurchaseFactoryInterface.h"

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

@interface AppleStoreInAppPurchasePaymentTransactionObserver : NSObject <SKPaymentTransactionObserver>

@property (assign) Mengine::AppleStoreInAppPurchaseFactoryInterface * _Nullable m_factory;
@property (assign) Mengine::AppleStoreInAppPurchaseServiceInterface * _Nullable m_service;

@property (strong) NSMutableArray<NSDictionary *> * _Nonnull m_cacheSKPaymentTransactions;

+ (instancetype _Nonnull) sharedInstance;

- (instancetype _Nonnull) init;

- (void)setupWithFactory: (Mengine::AppleStoreInAppPurchaseFactoryInterface * _Nonnull)_factory service: (Mengine::AppleStoreInAppPurchaseServiceInterface * _Nonnull)_service;

@end
