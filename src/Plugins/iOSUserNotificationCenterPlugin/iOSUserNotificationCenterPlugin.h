#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "Environment/iOS/iOSPluginConfigDelegateInterface.h"

#import <UserNotifications/UserNotifications.h>

NS_ASSUME_NONNULL_BEGIN

@interface iOSUserNotificationCenterPlugin : NSObject<iOSPluginInterface, iOSPluginConfigDelegateInterface, UNUserNotificationCenterDelegate>

+ (instancetype)sharedInstance;

- (void)requestAuthorization:(void (^ _Nonnull)(BOOL granted))completion;
- (void)queueNotificationsWithInterval:(NSTimeInterval)interval;
- (void)setNotificationDelay:(NSString *)identifier delay:(NSTimeInterval)delay;

@property (nonatomic, assign) BOOL m_notificationPermissionGranted;

@end

NS_ASSUME_NONNULL_END
