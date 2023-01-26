#import <OneSignal/OneSignal.h>

#import <UserNotifications/UserNotifications.h>

#include "Config/Config.h"

@interface AppleOneSignalNotificationServiceExtension : UNNotificationServiceExtension

@property (nonatomic, strong) void (^contentHandler)(UNNotificationContent *contentToDeliver);
@property (nonatomic, strong) UNNotificationRequest *receivedRequest;
@property (nonatomic, strong) UNMutableNotificationContent *bestAttemptContent;

@end
