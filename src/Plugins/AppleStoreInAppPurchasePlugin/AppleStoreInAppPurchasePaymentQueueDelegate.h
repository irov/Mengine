#pragma once

#include "AppleStoreInAppPurchaseInterface.h"
#include "AppleStoreInAppPurchaseFactoryInterface.h"

#import "Environment/Apple/AppleIncluder.h"

#import <StoreKit/StoreKit.h>

@interface AppleStoreInAppPurchasePaymentQueueDelegate : NSObject <SKPaymentQueueDelegate>

@end
