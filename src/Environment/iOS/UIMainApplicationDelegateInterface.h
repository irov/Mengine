#include "Config/Config.h"

#import "Environment/iOS/UIPluginApplicationDelegateInterface.h"

#import <Foundation/Foundation.h>

@protocol UIMainApplicationDelegateInterface <NSObject>

- (NSArray<UIPluginApplicationDelegateInterface> *)getPluginDelegates;

- (void)notify:(AppleEvent *)event args:(id)firstArg, ... NS_REQUIRES_NIL_TERMINATION;
- (void)notify:(AppleEvent *)event arrayArgs:(NSArray<id> *)args;

- (void)log:(const Mengine::LoggerRecordInterfacePtr &)record;

@end
