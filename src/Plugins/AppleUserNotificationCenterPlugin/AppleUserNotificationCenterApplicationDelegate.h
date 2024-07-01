#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import <UserNotifications/UserNotifications.h>

@interface AppleUserNotificationCenterApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, UNUserNotificationCenterDelegate>

@end
