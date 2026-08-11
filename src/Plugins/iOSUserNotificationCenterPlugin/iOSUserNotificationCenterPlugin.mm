#import "iOSUserNotificationCenterPlugin.h"

#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#include "Kernel/ScriptEmbeddingHelper.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "iOSUserNotificationCenterScriptEmbedding.h"
#endif

#import <UserNotifications/UserNotifications.h>

@implementation iOSUserNotificationCenterPlugin

+ (instancetype)sharedInstance {
    static iOSUserNotificationCenterPlugin *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[iOSUserNotificationCenterPlugin class]];
    });
    return sharedInstance;
}

- (void)requestAuthorization:(void (^ _Nonnull)(BOOL granted))completion {
    __weak iOSUserNotificationCenterPlugin * weakSelf = self;

    [iOSDetail addDidBecomeActiveOperationWithCompletion:^(void (^ _Nonnull operationCompletion)(void)) {
        iOSUserNotificationCenterPlugin * strongSelf = weakSelf;

        if (strongSelf == nil) {
            completion(NO);
            operationCompletion();
            return;
        }

        UNUserNotificationCenter * center = [UNUserNotificationCenter currentNotificationCenter];

        [center requestAuthorizationWithOptions:(UNAuthorizationOptionAlert | UNAuthorizationOptionSound | UNAuthorizationOptionBadge)
                              completionHandler:^(BOOL granted, NSError * _Nullable error) {
            [AppleDetail addMainQueueOperation:^{
                if (error != nil) {
                    IOS_LOGGER_MESSAGE(@"Notification permission request failed: %@"
                       , [AppleDetail getMessageFromNSError:error]
                    );

                    strongSelf.m_notificationPermissionGranted = NO;
                    completion(NO);
                    operationCompletion();
                    return;
                }

                strongSelf.m_notificationPermissionGranted = granted;
                completion(granted);
                operationCompletion();
            }];
        }];
    }];
}

- (void)scheduleNotification:(NSNumber *)badge withTitle:(NSString *)title withBody:(NSString *)body withDelay:(NSTimeInterval)delay relevanceScore:(double)relevanceScore {
    UNMutableNotificationContent * content = [[UNMutableNotificationContent alloc] init];
    content.title = title;
    content.body = body;
    content.sound = [UNNotificationSound defaultSound];
    content.badge = badge;
    content.interruptionLevel = UNNotificationInterruptionLevelTimeSensitive;
    content.relevanceScore = relevanceScore; // от 0.0 до 1.0

    UNTimeIntervalNotificationTrigger * trigger = [UNTimeIntervalNotificationTrigger triggerWithTimeInterval:delay repeats:NO];

    NSString * identifier = @"mengine_reminder";
    UNNotificationRequest * request = [UNNotificationRequest requestWithIdentifier:identifier content:content trigger:trigger];

    [[UNUserNotificationCenter currentNotificationCenter] addNotificationRequest:request withCompletionHandler:^(NSError * _Nullable error) {
        if (error != nil) {
            IOS_LOGGER_WARNING(@"error add notification request: %@"
                , [AppleDetail getMessageFromNSError:error]
            );

            return;
        }

        IOS_LOGGER_MESSAGE(@"notification successful add badge: %@ title: %@ body: %@ delay: %lf"
            , badge
            , title
            , body
            , delay
        );
    }];
}

#pragma mark - UIApplicationDelegate

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    UNUserNotificationCenter * center = [UNUserNotificationCenter currentNotificationCenter];

    center.delegate = self;

    return YES;
}

- (void)onRunBegin {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::addScriptEmbedding<Mengine::iOSUserNotificationCenterScriptEmbedding>( MENGINE_DOCUMENT_FUNCTION );
#endif
}

- (void)onStopEnd {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::removeScriptEmbedding<Mengine::iOSUserNotificationCenterScriptEmbedding>();
#endif
}

#pragma mark - UNUserNotificationCenterDelegate Protocol

- (void)userNotificationCenter:(UNUserNotificationCenter *)center willPresentNotification:(UNNotification *)notification withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler API_AVAILABLE(macos(10.14), ios(10.0), watchos(3.0), tvos(10.0)); {

    NSLog(@"userNotificationCenter willPresentNotification identifier: %@ title: %@ body: %@"
        , notification.request.identifier
        , notification.request.content.title
        , notification.request.content.body
    );

    completionHandler(UNNotificationPresentationOptionBanner | UNNotificationPresentationOptionList | UNNotificationPresentationOptionSound | UNNotificationPresentationOptionBadge);
}

- (void)userNotificationCenter:(UNUserNotificationCenter *)center didReceiveNotificationResponse:(UNNotificationResponse *)response withCompletionHandler:(void(^)(void))completionHandler API_AVAILABLE(macos(10.14), ios(10.0), watchos(3.0)) API_UNAVAILABLE(tvos) {

    NSLog(@"userNotificationCenter didReceiveNotificationResponse identifier: %@ title: %@ body: %@"
        , response.notification.request.identifier
        , response.notification.request.content.title
        , response.notification.request.content.body
    );

    completionHandler();
}

- (void)userNotificationCenter:(UNUserNotificationCenter *)center openSettingsForNotification:(nullable UNNotification *)notification API_AVAILABLE(macos(10.14), ios(12.0)) API_UNAVAILABLE(watchos, tvos) {

    NSLog(@"userNotificationCenter openSettingsForNotification");
}

@end
