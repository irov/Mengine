#pragma once

#include "Config/Config.h"

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

@interface iOSApplication : NSObject

+ (instancetype _Nonnull)sharedInstance;

+ (void)setPersistentStringForKey:(NSString * _Nonnull)key value:(NSString * _Nullable)value;
+ (NSString * _Nullable)getPersistentStringForKey:(NSString * _Nonnull)key defaultValue:(NSString * _Nullable)d;

+ (void)setPersistentIntegerForKey:(NSString * _Nonnull)key value:(NSInteger)value;
+ (NSInteger)getPersistentIntegerForKey:(NSString * _Nonnull)key defaultValue:(NSInteger)d;

- (BOOL)didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions;

- (void)setSessionId:(NSString * _Nonnull)sessionId;
- (void)removeSessionData;

- (NSString * _Nonnull)getInstallKey;
- (NSInteger)getInstallTimestamp;
- (NSString * _Nonnull)getInstallVersion;
- (NSInteger)getInstallRND;
- (NSInteger)getSessionIndex;
- (NSString * _Nonnull)getSessionId;
- (NSInteger)getSessionTimestamp;

@property (nonatomic, strong) NSString * _Nonnull m_installKey;
@property (nonatomic) NSInteger m_installTimestamp;
@property (nonatomic, strong) NSString * _Nonnull m_installVersion;
@property (nonatomic) NSInteger m_installRND;
@property (nonatomic) NSInteger m_sessionIndex;
@property (nonatomic, strong) NSString * _Nonnull m_sessionId;
@property (nonatomic) NSInteger m_sessionTimestamp;

@end
