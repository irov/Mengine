#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol iOSPluginConfigDelegateInterface <NSObject>

- (void)onConfig:(NSDictionary * _Nonnull)config ids:(NSDictionary<NSString *, NSNumber *> * _Nonnull)ids;

@end
