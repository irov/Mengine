#pragma once

#include "iOSStoreInAppPurchaseInterface.h"

#import "Environment/Apple/AppleIncluder.h"

#import <StoreKit/StoreKit.h>

@interface iOSStoreInAppPurchaseProductsRequestDelegate : NSObject <SKProductsRequestDelegate>

@property (nonatomic, weak) id<iOSStoreInAppPurchaseInterface> m_inAppPurchase;
@property (assign) Mengine::iOSStoreInAppPurchaseProductsRequestInterfacePtr m_request;
@property (assign) Mengine::iOSStoreInAppPurchaseProductsResponseInterfacePtr m_cb;

- (instancetype _Nonnull)initWithInAppPurchase:(id<iOSStoreInAppPurchaseInterface> _Nonnull)inAppPurchase
                                       request:(const Mengine::iOSStoreInAppPurchaseProductsRequestInterfacePtr &)request
                                            cb:(const Mengine::iOSStoreInAppPurchaseProductsResponseInterfacePtr &)cb;

@end
