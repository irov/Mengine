#pragma once

#import "Environment/Apple/AppleIncluder.h"

@interface AppleAdvertisementCooldown : NSObject

@property (nonatomic) NSInteger m_timestamp;

- (instancetype)init;

- (NSInteger)getLastShownTimestamp;
- (void)resetShownTimestamp;

@end
