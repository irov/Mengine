#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@protocol AppleGameCenterConnectCallbackInterface <NSObject>
- (void)onAppleGameCenterAuthenticate:(BOOL)successful;
- (void)onAppleGameCenterSynchronizate:(BOOL)successful;
@end

@protocol AppleGameCenterInterface <NSObject>

+ (instancetype _Nonnull)sharedInstance;

- (void)connect:(id<AppleGameCenterConnectCallbackInterface> _Nonnull)callback;
- (BOOL)isConnect;
- (BOOL)isSynchronized;

- (BOOL)reportAchievement:(NSString * _Nonnull)identifier percent:(double)percent response:(void(^ _Nonnull)(BOOL))handler;
- (BOOL)checkAchievement:(NSString * _Nonnull)identifier;

- (BOOL)login:(void(^ _Nonnull)(NSError* _Nullable))handler;
- (BOOL)loadCompletedAchievements:(void(^ _Nonnull)(NSError * _Nullable, NSArray* _Nullable))handler;
- (BOOL)resetAchievements:(void(^ _Nonnull)(NSError * __nullable error))handler;

- (BOOL)reportScore:(NSString * _Nonnull)identifier score:(int64_t)score response:(void(^ _Nonnull)(NSError * _Nullable))handler;
- (BOOL)reportAchievementIdentifier:(NSString * _Nonnull)identifier percentComplete:(double)percent withBanner:(BOOL)banner response:(void(^ _Nonnull)(NSError * _Nullable))handler;

@end
