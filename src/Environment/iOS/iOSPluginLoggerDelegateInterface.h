#include "Config/Config.h"

#import "Environment/iOS/iOSLogRecordParam.h"

#import <Foundation/Foundation.h>

@protocol iOSPluginLoggerDelegateInterface <NSObject>

- (void)onLogger:(iOSLogRecordParam *)record;

@end
