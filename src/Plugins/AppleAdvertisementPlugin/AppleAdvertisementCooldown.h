#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@interface AppleAdvertisementCooldown : NSObject

@property (nonatomic) NSTimeInterval m_timestamp;

- (instancetype)init;

- (NSTimeInterval)getLastShownTimestamp;
- (void)resetShownTimestamp;

@end
