#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#include "AppleStoreInAppPurchaseInterface.h"

#import "Environment/Apple/AppleIncluder.h"

#import <StoreKit/StoreKit.h>

@interface AppleStoreInAppPurchaseApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, SKPaymentQueueDelegate, AppleStoreInAppPurchaseInterface>

@end
