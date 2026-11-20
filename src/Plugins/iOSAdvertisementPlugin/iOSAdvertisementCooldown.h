#pragma once

#import "Environment/Apple/AppleIncluder.h"

@interface iOSAdvertisementCooldown : NSObject

- (instancetype)initWithPreferenceKey:(NSString *)_preferenceKey;

- (NSInteger)getLastShownTimestamp;
- (void)resetShownTimestamp;
- (void)save;

@end
