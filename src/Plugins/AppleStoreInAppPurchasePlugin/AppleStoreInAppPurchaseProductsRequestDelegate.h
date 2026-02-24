#pragma once

#include "AppleStoreInAppPurchaseInterface.h"

#import "Environment/Apple/AppleIncluder.h"

#import <StoreKit/StoreKit.h>

@interface AppleStoreInAppPurchaseProductsRequestDelegate : NSObject <SKProductsRequestDelegate>

@property (nonatomic, weak) id<AppleStoreInAppPurchaseInterface> _Nonnull m_inAppPurchase;
@property (assign) Mengine::AppleStoreInAppPurchaseProductsRequestInterfacePtr m_request;
@property (assign) Mengine::AppleStoreInAppPurchaseProductsResponseInterfacePtr m_cb;

- (instancetype _Nonnull)initWithInAppPurchase:(id<AppleStoreInAppPurchaseInterface> _Nonnull)inAppPurchase
                                       request:(const Mengine::AppleStoreInAppPurchaseProductsRequestInterfacePtr &)request
                                            cb:(const Mengine::AppleStoreInAppPurchaseProductsResponseInterfacePtr &)cb;

@end
