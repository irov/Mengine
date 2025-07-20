#pragma once

#import "Environment/Apple/AppleIncluder.h"

@interface AppleAdvertisementAttempts : NSObject

@property (nonatomic) NSInteger m_attempts;

- (instancetype)init;

- (NSInteger)getAttempts;
- (NSInteger)incrementAttempts;

@end
