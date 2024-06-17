#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@interface MacOSApplication : NSObject

+ (instancetype _Nonnull) sharedInstance;

- (void)initialize;

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
