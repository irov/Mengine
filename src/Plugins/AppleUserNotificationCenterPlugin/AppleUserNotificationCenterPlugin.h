#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

#import <UserNotifications/UserNotifications.h>

@interface AppleUserNotificationCenterPlugin : NSObject<iOSPluginInterface, UNUserNotificationCenterDelegate>

+ (instancetype)sharedInstance;

- (void)scheduleNotification:(NSNumber *)badge withTitle:(NSString *)title withBody:(NSString *)body withDelay:(NSTimeInterval)delay relevanceScore:(double)relevanceScore;

@property (nonatomic, assign) BOOL m_notificationPermissionGranted;

@end
