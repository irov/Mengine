#include "AppleUserNotificationCenterService.h"

#import "Environment/Apple/AppleDetail.h"

#include "Kernel/Logger.h"

#import <UserNotifications/UserNotifications.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleUserNotificationCenterService, Mengine::AppleUserNotificationCenterService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleUserNotificationCenterService::AppleUserNotificationCenterService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleUserNotificationCenterService::~AppleUserNotificationCenterService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleUserNotificationCenterService::_initializeService()
    {
        UNUserNotificationCenter * center = [UNUserNotificationCenter currentNotificationCenter];
        
        [center requestAuthorizationWithOptions:(UNAuthorizationOptionAlert | UNAuthorizationOptionSound | UNAuthorizationOptionBadge)
                              completionHandler:^(BOOL granted, NSError * _Nullable error) {
            m_notificationPermissionGranted = granted;
            
            if (error != nil) {
                LOGGER_MESSAGE("Notification permission denied: %s"
                   , [[AppleDetail getMessageFromNSError:error] UTF8String]
                );
                
                return;
            }
        }];
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleUserNotificationCenterService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleUserNotificationCenterService::scheduleNotification( int32_t _badge, NSString * _title, NSString * _body, Timestamp _delay )
    {
        UNMutableNotificationContent * content = [[UNMutableNotificationContent alloc] init];
        content.title = _title;
        content.body = _body;
        content.sound = [UNNotificationSound defaultSound];
        content.badge = @(_badge);

        UNTimeIntervalNotificationTrigger * trigger = [UNTimeIntervalNotificationTrigger triggerWithTimeInterval:(NSTimeInterval)_delay repeats:NO];

        NSString * identifier = @"mengine_reminder";
        UNNotificationRequest * request = [UNNotificationRequest requestWithIdentifier:identifier content:content trigger:trigger];

        [[UNUserNotificationCenter currentNotificationCenter] addNotificationRequest:request withCompletionHandler:^(NSError * _Nullable error) {
            if (error != nil) {
                LOGGER_WARNING("error add notification request: %s"
                    , [[AppleDetail getMessageFromNSError:error] UTF8String]
                );
                
                return;
            }
            
            LOGGER_MESSAGE("notification successful add badge: %d title: %s body: %s delay: %llu"
                , _badge
                , [_title UTF8String]
                , [_body UTF8String]
                , _delay
            );
        }];
    }
    //////////////////////////////////////////////////////////////////////////
}
