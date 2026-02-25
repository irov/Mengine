#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

#import "AppleGameCenterInterface.h"

#import <GameKit/GameKit.h>

@interface AppleGameCenterPlugin : NSObject<iOSPluginInterface, AppleGameCenterInterface, GKGameCenterControllerDelegate>

@property (assign) BOOL m_authenticateSuccess;
@property (assign) BOOL m_gameCenterAuthenticate;
@property (assign) BOOL m_achievementsSynchronization;
@property (strong) NSMutableArray<NSString *> * m_achievementsComplete;

@end
