#import "Environment/iOS/UIProxyApplicationDelegateInterface.h"

#import <UserNotifications/UserNotifications.h>

@interface AppleUserNotificationCenterApplicationDelegate : NSObject<UIProxyApplicationDelegateInterface, UNUserNotificationCenterDelegate>

@end
