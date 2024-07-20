#include "Config/Config.h"

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginLoggerDelegateInterface.h"
#import "Environment/iOS/iOSPluginAdRevenueDelegateInterface.h"
#import "Environment/iOS/iOSPluginTransparencyConsentDelegateInterface.h"

#import <Foundation/Foundation.h>

@protocol iOSUIMainApplicationDelegateInterface <NSObject>

- (NSArray<iOSPluginApplicationDelegateInterface> *)getPluginApplicationDelegates;
- (NSArray<iOSPluginLoggerDelegateInterface> *)getPluginLoggerDelegates;
- (NSArray<iOSPluginAdRevenueDelegateInterface> *)getPluginAdRevenueDelegates;
- (NSArray<iOSPluginTransparencyConsentDelegateInterface> *)getPluginTransparencyConsentDelegates;

- (void)addMainQueueOperation:(dispatch_block_t)block;

- (void)notify:(AppleEvent *)event args:(id)firstArg, ... NS_REQUIRES_NIL_TERMINATION;
- (void)notify:(AppleEvent *)event arrayArgs:(NSArray<id> *)args;

- (void)log:(iOSLogRecordParam *)record;

- (void)eventAdRevenue:(iOSAdRevenueParam *)revenue;
- (void)eventTransparencyConsent:(iOSTransparencyConsentParam *)consent;

@end
