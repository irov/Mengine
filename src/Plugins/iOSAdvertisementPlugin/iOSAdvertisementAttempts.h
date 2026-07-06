#pragma once

#import "Environment/Apple/AppleIncluder.h"

@interface iOSAdvertisementAttempts : NSObject

@property (nonatomic) NSInteger m_attempts;

- (instancetype)init;

- (NSInteger)getAttempts;
- (NSInteger)incrementAttempts;

@end
