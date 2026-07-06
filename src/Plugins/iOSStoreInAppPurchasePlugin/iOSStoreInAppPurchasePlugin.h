#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

#import "iOSStoreInAppPurchaseInterface.h"

#import "Environment/Apple/AppleIncluder.h"

#import <StoreKit/StoreKit.h>

@interface iOSStoreInAppPurchasePlugin : NSObject<iOSPluginInterface, SKPaymentQueueDelegate, iOSStoreInAppPurchaseInterface>

@end
