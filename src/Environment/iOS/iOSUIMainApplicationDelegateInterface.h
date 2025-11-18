#pragma once

#import "Environment/Apple/AppleIncluder.h"
#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginLoggerDelegateInterface.h"
#import "Environment/iOS/iOSPluginConfigDelegateInterface.h"
#import "Environment/iOS/iOSPluginUserIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginAdRevenueDelegateInterface.h"
#import "Environment/iOS/iOSPluginAppTrackingTransparencyDelegateInterface.h"
#import "Environment/iOS/iOSPluginTransparencyConsentDelegateInterface.h"

@protocol iOSUIMainApplicationDelegateInterface <NSObject>

- (NSArray<iOSPluginApplicationDelegateInterface> * _Nonnull)getPluginApplicationDelegates;
- (NSArray<iOSPluginLoggerDelegateInterface> * _Nonnull)getPluginLoggerDelegates;
- (NSArray<iOSPluginConfigDelegateInterface> * _Nonnull)getPluginConfigDelegates;
- (NSArray<iOSPluginUserIdDelegateInterface> * _Nonnull)getPluginUserIdDelegates;
- (NSArray<iOSPluginAdRevenueDelegateInterface> * _Nonnull)getPluginAdRevenueDelegates;
- (NSArray<iOSPluginAppTrackingTransparencyDelegateInterface> *_Nonnull)getPluginAppTrackingTransparencyDelegates;
- (NSArray<iOSPluginTransparencyConsentDelegateInterface> * _Nonnull)getPluginTransparencyConsentDelegates;

- (id _Nullable)getPluginDelegateOfClass:(Class _Nonnull)delegateClass;
- (id _Nullable)getPluginDelegateOfProtocol:(Protocol * _Nonnull)protocol;

- (void)notify:(AppleEvent * _Nonnull)event args:(id _Nonnull)firstArg, ... NS_REQUIRES_NIL_TERMINATION;
- (void)notify:(AppleEvent * _Nonnull)event arrayArgs:(NSArray<id> * _Nonnull)args;

- (void)eventLog:(AppleLogRecordParam * _Nonnull)record;

- (void)eventConfig:(NSDictionary * _Nonnull)config;

- (void)eventAnalytic:(NSString * _Nonnull)event params:(NSDictionary * _Nonnull)params;
- (void)eventAnalyticScreen:(NSString * _Nonnull)name type:(NSString * _Nonnull)type;
- (void)eventAnalyticFlush;

- (void)eventSetUserId:(iOSUserParam * _Nonnull)userId;
- (void)eventRemoveUserData;

- (void)eventAdRevenue:(iOSAdRevenueParam * _Nonnull)revenue;
- (void)eventAppTrackingTransparency:(iOSAppTrackingTransparencyParam * _Nonnull)tracking;
- (void)eventTransparencyConsent:(iOSTransparencyConsentParam * _Nonnull)consent;

- (void)addDidBecomeActiveOperation:(dispatch_block_t _Nonnull)block;

@end
