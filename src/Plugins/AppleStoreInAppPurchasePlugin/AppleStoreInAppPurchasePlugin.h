#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#include "AppleStoreInAppPurchaseInterface.h"

#import "Environment/Apple/AppleIncluder.h"

#import <StoreKit/StoreKit.h>

@interface AppleStoreInAppPurchasePlugin : NSObject<iOSPluginInterface, SKPaymentQueueDelegate, AppleStoreInAppPurchaseInterface>

@end
