#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol iOSPluginAttributionDelegateInterface <NSObject>

- (void)onAttribution:(NSString * _Nonnull)name value:(id _Nonnull)value;

@end
