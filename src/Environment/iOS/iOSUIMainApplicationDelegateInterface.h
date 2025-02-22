#include "Config/Config.h"

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginLoggerDelegateInterface.h"
#import "Environment/iOS/iOSPluginSessionIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginAdRevenueDelegateInterface.h"
#import "Environment/iOS/iOSPluginTransparencyConsentDelegateInterface.h"

#import <Foundation/Foundation.h>

@protocol iOSUIMainApplicationDelegateInterface <NSObject>

- (NSArray<iOSPluginApplicationDelegateInterface> * _Nonnull) getPluginApplicationDelegates;
- (NSArray<iOSPluginLoggerDelegateInterface> * _Nonnull) getPluginLoggerDelegates;
- (NSArray<iOSPluginSessionIdDelegateInterface> * _Nonnull) getPluginSessionIdDelegates;
- (NSArray<iOSPluginAdRevenueDelegateInterface> * _Nonnull) getPluginAdRevenueDelegates;
- (NSArray<iOSPluginTransparencyConsentDelegateInterface> * _Nonnull) getPluginTransparencyConsentDelegates;

- (id _Nullable) getPluginDelegateOfClass:(Class _Nonnull)delegateClass;

- (void) notify:(AppleEvent * _Nonnull)event args:(id _Nonnull)firstArg, ... NS_REQUIRES_NIL_TERMINATION;
- (void) notify:(AppleEvent * _Nonnull)event arrayArgs:(NSArray<id> * _Nonnull)args;

- (void) log:(AppleLogRecordParam * _Nonnull)record;

- (void) setSessionId:(iOSSessionIdParam * _Nonnull)sessionId;
- (void) removeSessionData;
- (void) eventAdRevenue:(iOSAdRevenueParam * _Nonnull)revenue;
- (void) eventTransparencyConsent:(iOSTransparencyConsentParam * _Nonnull)consent;

@end
