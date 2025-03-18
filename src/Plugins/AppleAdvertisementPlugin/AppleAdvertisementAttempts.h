#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@interface AppleAdvertisementAttempts : NSObject

@property (nonatomic) NSInteger m_attempts;

- (instancetype)init;

- (NSInteger)getAttempts;
- (NSInteger)incrementAttempts;

@end
