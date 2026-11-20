#pragma once

#import "Environment/Apple/AppleIncluder.h"

@interface iOSAdvertisementFrequency : NSObject

- (instancetype)initWithPlacement:(NSString *)_placement;

- (BOOL)hasShowQuota:(NSInteger)_showLimit withTimeout:(NSInteger)_showTimeout;
- (void)recordShownWithTimeout:(NSInteger)_showTimeout;
- (void)save;

@end
