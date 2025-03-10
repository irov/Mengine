#pragma once

#import "Interface/AppleAdvertisementInterface.h"

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>

@protocol AppleGameCenterConnectProviderInterface <NSObject>
- (void)onAppleGameCenterAuthenticate:(BOOL)successful;
- (void)onAppleGameCenterSynchronizate:(BOOL)successful;
@end

@interface AppleGameCenterApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, GKGameCenterControllerDelegate>

+ (instancetype _Nonnull)sharedInstance;

- (instancetype _Nonnull)init;

- (void)connect:(id<AppleGameCenterConnectProviderInterface> _Nonnull)provider;
- (BOOL)isConnect;
- (BOOL)isSynchronized;

- (BOOL)reportAchievement:(NSString * _Nonnull)identifier percent:(double)percent response:(void(^ _Nonnull)(BOOL))handler;
- (BOOL)checkAchievement:(NSString * _Nonnull)identifier;

- (BOOL)login:(void(^ _Nonnull)(NSError* _Nullable))handler;
- (BOOL)loadCompletedAchievements:(void(^ _Nonnull)(NSError * _Nullable, NSArray* _Nullable))handler;
- (BOOL)resetAchievements:(void(^ _Nonnull)(NSError * __nullable error))handler;

- (BOOL)reportScore:(NSString * _Nonnull)identifier score:(int64_t)score response:(void(^ _Nonnull)(NSError * _Nullable))handler;
- (BOOL)reportAchievementIdentifier:(NSString * _Nonnull)identifier percentComplete:(double)percent withBanner:(BOOL)banner response:(void(^ _Nonnull)(NSError * _Nullable))handler;

@property (assign) BOOL m_authenticateSuccess;
@property (assign) BOOL m_gameCenterAuthenticate;
@property (assign) BOOL m_achievementsSynchronization;
@property (strong) NSMutableArray<NSString *> * _Nonnull m_achievementsComplete;

@end
