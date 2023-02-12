#import "Interface/UIKitProxyApplicationDelegateInterface.h"

#import <UserNotifications/UserNotifications.h>

@interface AppleUserNotificationCenterApplicationDelegate : NSObject<UIKitProxyApplicationDelegateInterface, UNUserNotificationCenterDelegate>

@end
