#include "Config/Config.h"

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginLoggerDelegateInterface.h"
#import "Environment/iOS/iOSPluginConfigDelegateInterface.h"
#import "Environment/iOS/iOSPluginSessionIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginAdRevenueDelegateInterface.h"
#import "Environment/iOS/iOSPluginTransparencyConsentDelegateInterface.h"

#import <Foundation/Foundation.h>

@protocol iOSUIMainApplicationDelegateInterface <NSObject>

- (NSArray<iOSPluginApplicationDelegateInterface> * _Nonnull)getPluginApplicationDelegates;
- (NSArray<iOSPluginLoggerDelegateInterface> * _Nonnull)getPluginLoggerDelegates;
- (NSArray<iOSPluginConfigDelegateInterface> * _Nonnull)getPluginConfigDelegates;
- (NSArray<iOSPluginSessionIdDelegateInterface> * _Nonnull)getPluginSessionIdDelegates;
- (NSArray<iOSPluginAdRevenueDelegateInterface> * _Nonnull)getPluginAdRevenueDelegates;
- (NSArray<iOSPluginTransparencyConsentDelegateInterface> * _Nonnull)getPluginTransparencyConsentDelegates;

- (id _Nullable)getPluginDelegateOfClass:(Class _Nonnull)delegateClass;
- (id _Nullable)getPluginDelegateOfProtocol:(Protocol * _Nonnull)protocol;

- (void)notify:(AppleEvent * _Nonnull)event args:(id _Nonnull)firstArg, ... NS_REQUIRES_NIL_TERMINATION;
- (void)notify:(AppleEvent * _Nonnull)event arrayArgs:(NSArray<id> * _Nonnull)args;

- (void)log:(AppleLogRecordParam * _Nonnull)record;

- (void)config:(NSDictionary * _Nonnull)config;

- (void)analyticEvent:(NSString * _Nonnull)event params:(NSDictionary * _Nonnull)params;
- (void)analyticScreen:(NSString * _Nonnull)name type:(NSString * _Nonnull)type;
- (void)analyticFlush;

- (void)setSessionId:(iOSSessionIdParam * _Nonnull)sessionId;
- (void)removeSessionData;
- (void)eventAdRevenue:(iOSAdRevenueParam * _Nonnull)revenue;
- (void)eventTransparencyConsent:(iOSTransparencyConsentParam * _Nonnull)consent;

@end
