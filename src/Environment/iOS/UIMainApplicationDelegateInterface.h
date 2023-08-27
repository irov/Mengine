#include "Config/Config.h"

#import "Environment/iOS/UIProxyApplicationDelegateInterface.h"

#import <Foundation/Foundation.h>

@protocol UIMainApplicationDelegateInterface <NSObject>

- (NSArray<UIProxyApplicationDelegateInterface> *)getPluginDelegates;

- (void)notify:(NSString *)name args:(id)firstArg, ... NS_REQUIRES_NIL_TERMINATION;
- (void)notify:(NSString *)name arrayArgs:(NSArray<id> *)args;

@end
