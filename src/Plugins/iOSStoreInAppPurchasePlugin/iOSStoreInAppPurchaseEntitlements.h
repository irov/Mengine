#pragma once

#import "Environment/Apple/AppleIncluder.h"

@interface iOSStoreInAppPurchaseEntitlements : NSObject

+ (instancetype _Nonnull) sharedInstance;

- (void)add:(NSString * _Nonnull)productIdentifier;
- (void)remove:(NSString * _Nonnull)productIdentifier;
- (BOOL)isPurchased:(NSString * _Nonnull)productIdentifier;

@end
