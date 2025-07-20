#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol iOSPluginAnalyticDelegateInterface <NSObject>

- (void)onAnalyticEvent:(NSString * _Nonnull)event params:(NSDictionary * _Nonnull)params;
- (void)onAnalyticScreen:(NSString * _Nonnull)screen type:(NSString * _Nonnull)type;
- (void)onAnalyticFlush;

@end
