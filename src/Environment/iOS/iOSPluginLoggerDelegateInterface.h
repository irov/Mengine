#include "Config/Config.h"

#include "Interface/LoggerRecordInterface.h"

#import <Foundation/Foundation.h>

@protocol iOSPluginLoggerDelegateInterface <NSObject>

- (void)onLogger:(const Mengine::LoggerRecordInterfacePtr &)record;

@end
