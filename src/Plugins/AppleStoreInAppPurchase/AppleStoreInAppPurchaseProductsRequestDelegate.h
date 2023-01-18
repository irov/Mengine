#include "AppleStoreInAppPurchaseInterface.h"
#include "AppleStoreInAppPurchaseFactoryInterface.h"

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

@interface AppleStoreInAppPurchaseProductsRequestDelegate : NSObject <SKProductsRequestDelegate>

@property (assign) Mengine::AppleStoreInAppPurchaseFactoryInterface * _Nonnull m_factory;
@property (assign) Mengine::AppleStoreInAppPurchaseProductsResponseInterfacePtr m_cb;

- (instancetype _Nonnull)initWithFactory: (Mengine::AppleStoreInAppPurchaseFactoryInterface * _Nonnull)_factory cb: (Mengine::AppleStoreInAppPurchaseProductsResponseInterfacePtr)_cb;

@end
