#pragma once

#import "Environment/Apple/AppleIncluder.h"

#import <UIKit/UIKit.h>

@interface iOSApplication : NSObject

+ (instancetype _Nonnull)sharedInstance;

- (BOOL)didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions;

- (void)setUserId:(NSString * _Nonnull)userId;
- (void)removeUserData;

- (NSString * _Nonnull)getInstallId;
- (NSInteger)getInstallTimestamp;
- (NSString * _Nonnull)getInstallVersion;
- (NSInteger)getInstallRND;
- (NSInteger)getSessionIndex;
- (NSString * _Nonnull)getSessionId;
- (NSInteger)getSessionTimestamp;
- (NSInteger)getSessionRND;

- (NSString * _Nonnull)getUserId;

@property (nonatomic, strong) NSString * _Nonnull m_installId;
@property (nonatomic) NSInteger m_installTimestamp;
@property (nonatomic, strong) NSString * _Nonnull m_installVersion;
@property (nonatomic) NSInteger m_installRND;
@property (nonatomic) NSInteger m_sessionIndex;
@property (nonatomic, strong) NSString * _Nonnull m_sessionId;
@property (nonatomic) NSInteger m_sessionTimestamp;
@property (nonatomic) NSInteger m_sessionRND;

@property (nonatomic, strong) NSString * _Nonnull m_userId;

@end
