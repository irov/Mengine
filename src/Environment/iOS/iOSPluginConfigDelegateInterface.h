#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol iOSPluginConfigDelegateInterface <NSObject>

- (void)onConfig:(NSDictionary * _Nonnull)config ids:(NSDictionary * _Nonnull)ids;

@end
