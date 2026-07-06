#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol iOSGameCenterConnectCallbackInterface <NSObject>
- (void)oniOSGameCenterAuthenticate:(BOOL)successful;
- (void)oniOSGameCenterSynchronize:(BOOL)successful;
@end

@protocol iOSGameCenterInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)connect:(id<iOSGameCenterConnectCallbackInterface>)callback;
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
