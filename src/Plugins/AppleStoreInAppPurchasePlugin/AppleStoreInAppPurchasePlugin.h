#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

#import "AppleStoreInAppPurchaseInterface.h"

#import "Environment/Apple/AppleIncluder.h"

#include "Interface/FactoryInterface.h"

#import <StoreKit/StoreKit.h>

@interface AppleStoreInAppPurchasePlugin : NSObject<iOSPluginInterface, SKPaymentQueueDelegate, AppleStoreInAppPurchaseInterface>

@property (nonatomic, assign) Mengine::AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr m_paymentTransactionProvider;

@property (nonatomic, assign) Mengine::FactoryInterfacePtr m_factoryPaymentTransaction;
@property (nonatomic, assign) Mengine::FactoryInterfacePtr m_factoryProduct;
@property (nonatomic, assign) Mengine::FactoryInterfacePtr m_factoryProductsRequest;

@end
