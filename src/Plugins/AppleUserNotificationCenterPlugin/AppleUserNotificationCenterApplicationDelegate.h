#import "Environment/iOS/UIKitProxyApplicationDelegateInterface.h"

#import <UserNotifications/UserNotifications.h>

@interface AppleUserNotificationCenterApplicationDelegate : NSObject<UIKitProxyApplicationDelegateInterface, UNUserNotificationCenterDelegate>

@end
