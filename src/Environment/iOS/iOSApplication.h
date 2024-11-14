#pragma once

#include "Config/Config.h"

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

@interface iOSApplication : NSObject

+ (instancetype _Nonnull) sharedInstance;

- (BOOL)didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions;

- (void)setSessionId:(NSString * _Nonnull)sessionId;
- (void)removeSessionData;

- (NSString * _Nonnull)getInstallKey;
- (NSInteger)getInstallTimestamp;
- (NSString * _Nonnull)getInstallVersion;
- (NSInteger)getInstallRND;
- (NSInteger)getSessionIndex;
- (NSString * _Nonnull)getSessionId;

@property (nonatomic, strong) NSString * _Nonnull m_installKey;
@property (nonatomic) NSInteger m_installTimestamp;
@property (nonatomic, strong) NSString * _Nonnull m_installVersion;
@property (nonatomic) NSInteger m_installRND;
@property (nonatomic) NSInteger m_sessionIndex;
@property (nonatomic, strong) NSString * _Nonnull m_sessionId;

@end
