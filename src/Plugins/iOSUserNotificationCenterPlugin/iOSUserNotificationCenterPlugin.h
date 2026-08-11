#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

#import <UserNotifications/UserNotifications.h>

NS_ASSUME_NONNULL_BEGIN

@interface iOSUserNotificationCenterPlugin : NSObject<iOSPluginInterface, UNUserNotificationCenterDelegate>

+ (instancetype)sharedInstance;

- (void)requestAuthorization:(void (^ _Nonnull)(BOOL granted))completion;
- (void)scheduleNotification:(NSNumber *)badge withTitle:(NSString *)title withBody:(NSString *)body withDelay:(NSTimeInterval)delay relevanceScore:(double)relevanceScore;

@property (nonatomic, assign) BOOL m_notificationPermissionGranted;

@end

NS_ASSUME_NONNULL_END
