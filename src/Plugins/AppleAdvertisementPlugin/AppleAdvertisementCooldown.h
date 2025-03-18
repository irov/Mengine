#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@interface AppleAdvertisementCooldown : NSObject

@property (nonatomic) NSInteger m_timestamp;

- (instancetype)init;

- (NSInteger)getLastShownTimestamp;
- (void)resetShownTimestamp;

@end
