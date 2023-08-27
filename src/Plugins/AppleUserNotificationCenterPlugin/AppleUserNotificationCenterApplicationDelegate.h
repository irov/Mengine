#import "Environment/iOS/UIPluginApplicationDelegateInterface.h"

#import <UserNotifications/UserNotifications.h>

@interface AppleUserNotificationCenterApplicationDelegate : NSObject<UIPluginApplicationDelegateInterface, UNUserNotificationCenterDelegate>

@end
