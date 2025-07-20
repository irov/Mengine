#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol AppleGameCenterConnectCallbackInterface <NSObject>
- (void)onAppleGameCenterAuthenticate:(BOOL)successful;
- (void)onAppleGameCenterSynchronizate:(BOOL)successful;
@end

@protocol AppleGameCenterInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)connect:(id<AppleGameCenterConnectCallbackInterface>)callback;
- (BOOL)isConnect;
- (BOOL)isSynchronized;

- (BOOL)reportAchievement:(NSString *)identifier percent:(double)percent response:(void(^)(BOOL))handler;
- (BOOL)checkAchievement:(NSString *)identifier;

- (BOOL)login:(void(^)(NSError *))handler;
- (BOOL)loadCompletedAchievements:(void(^)(NSError *, NSArray*))handler;
- (BOOL)resetAchievements:(void(^)(NSError * error))handler;

- (BOOL)reportScore:(NSString *)identifier score:(int64_t)score response:(void(^)(NSError *))handler;
- (BOOL)reportAchievementIdentifier:(NSString *)identifier percentComplete:(double)percent withBanner:(BOOL)banner response:(void(^)(NSError *))handler;

@end
