#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import <UserNotifications/UserNotifications.h>

@interface AppleUserNotificationCenterApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, UNUserNotificationCenterDelegate>

+ (instancetype) sharedInstance;

- (void)scheduleNotification:(NSNumber *)badge withTitle:(NSString *)title withBody:(NSString *)body withDelay:(NSTimeInterval)delay;

@property (nonatomic, assign) BOOL m_notificationPermissionGranted;

@end
