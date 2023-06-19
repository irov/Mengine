#include "AppleStoreInAppPurchaseInterface.h"
#include "AppleStoreInAppPurchaseFactoryInterface.h"

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

@interface AppleStoreInAppPurchasePaymentQueueDelegate : NSObject <SKPaymentQueueDelegate>

@property (assign) Mengine::AppleStoreInAppPurchaseFactoryInterface * _Nonnull m_factory;
@property (assign) Mengine::AppleStoreInAppPurchaseServiceInterface * _Nonnull m_service;

- (instancetype _Nonnull) initWithFactory: (Mengine::AppleStoreInAppPurchaseFactoryInterface * _Nonnull)_factory service: (Mengine::AppleStoreInAppPurchaseServiceInterface * _Nonnull)_service;

@end
