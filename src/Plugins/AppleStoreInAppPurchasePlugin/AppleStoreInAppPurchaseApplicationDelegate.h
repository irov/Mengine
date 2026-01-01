#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import "Environment/Apple/AppleIncluder.h"

#import <StoreKit/StoreKit.h>

@interface AppleStoreInAppPurchaseApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, SKPaymentQueueDelegate>

@end
