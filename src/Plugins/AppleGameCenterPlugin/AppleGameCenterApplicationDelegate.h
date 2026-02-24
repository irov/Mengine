#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import "AppleGameCenterInterface.h"

#import <GameKit/GameKit.h>

@interface AppleGameCenterApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, AppleGameCenterInterface, GKGameCenterControllerDelegate>

@property (assign) BOOL m_authenticateSuccess;
@property (assign) BOOL m_gameCenterAuthenticate;
@property (assign) BOOL m_achievementsSynchronization;
@property (strong) NSMutableArray<NSString *> * m_achievementsComplete;

@end
