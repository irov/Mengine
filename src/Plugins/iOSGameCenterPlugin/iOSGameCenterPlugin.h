#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

#import "iOSGameCenterInterface.h"

#import <GameKit/GameKit.h>

@interface iOSGameCenterPlugin : NSObject<iOSPluginInterface, iOSGameCenterInterface, GKGameCenterControllerDelegate>

@property (assign) BOOL m_authenticateSuccess;
@property (assign) BOOL m_gameCenterAuthenticate;
@property (assign) BOOL m_achievementsSynchronization;
@property (strong) NSMutableArray<NSString *> * m_achievementsComplete;

@end
