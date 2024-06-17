#include "Config/Config.h"

#import "Environment/iOS/UIPluginApplicationDelegateInterface.h"

#import <Foundation/Foundation.h>

@protocol UIMainApplicationDelegateInterface <NSObject>

- (NSArray<UIPluginApplicationDelegateInterface> *)getPluginDelegates;

- (void)notify:(MengineEvent *)event args:(id)firstArg, ... NS_REQUIRES_NIL_TERMINATION;
- (void)notify:(MengineEvent *)event arrayArgs:(NSArray<id> *)args;

- (void)log:(const Mengine::LoggerRecordInterfacePtr &)record;

@end
