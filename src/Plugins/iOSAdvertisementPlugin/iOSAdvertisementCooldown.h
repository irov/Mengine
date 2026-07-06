#pragma once

#import "Environment/Apple/AppleIncluder.h"

@interface iOSAdvertisementCooldown : NSObject

@property (nonatomic) NSInteger m_timestamp;

- (instancetype)init;

- (NSInteger)getLastShownTimestamp;
- (void)resetShownTimestamp;

@end
