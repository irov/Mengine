#include "Config/Config.h"

#import "Environment/Apple/AppleLogRecordParam.h"

#import <Foundation/Foundation.h>

@protocol iOSPluginLoggerDelegateInterface <NSObject>

- (void)onLogger:(AppleLogRecordParam * _Nonnull)record;

@end
