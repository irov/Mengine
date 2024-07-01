#include "Config/Config.h"

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import <Foundation/Foundation.h>

@protocol iOSUIMainApplicationDelegateInterface <NSObject>

- (NSArray<iOSPluginApplicationDelegateInterface> *)getPluginApplicationDelegates;
- (NSArray<iOSPluginAdRevenueDelegateInterface> *)getPluginAdRevenueDelegates;

- (void)notify:(AppleEvent *)event args:(id)firstArg, ... NS_REQUIRES_NIL_TERMINATION;
- (void)notify:(AppleEvent *)event arrayArgs:(NSArray<id> *)args;

- (void)log:(const Mengine::LoggerRecordInterfacePtr &)record;

- (void)eventAdRevenue:(iOSAdRevenueParam *)revenue;

@end
