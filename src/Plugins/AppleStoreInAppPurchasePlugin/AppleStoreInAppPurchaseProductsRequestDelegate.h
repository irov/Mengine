#include "AppleStoreInAppPurchaseInterface.h"
#include "AppleStoreInAppPurchaseFactoryInterface.h"

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

@interface AppleStoreInAppPurchaseProductsRequestDelegate : NSObject <SKProductsRequestDelegate>

@property (assign) Mengine::AppleStoreInAppPurchaseFactoryInterface * _Nonnull m_factory;
@property (assign) Mengine::AppleStoreInAppPurchaseProductsRequestInterfacePtr m_request;
@property (assign) Mengine::AppleStoreInAppPurchaseProductsResponseInterfacePtr m_cb;

- (instancetype _Nonnull) initWithFactory: (Mengine::AppleStoreInAppPurchaseFactoryInterface * _Nonnull)_factory request:(const Mengine::AppleStoreInAppPurchaseProductsRequestInterfacePtr &)_request cb: (const Mengine::AppleStoreInAppPurchaseProductsResponseInterfacePtr &)_cb;

@end
