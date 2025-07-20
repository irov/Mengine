#pragma once

#import "Environment/Apple/AppleIncluder.h"
#import "Environment/Apple/AppleLogRecordParam.h"

@protocol iOSPluginLoggerDelegateInterface <NSObject>

- (void)onLogger:(AppleLogRecordParam * _Nonnull)record;

@end
