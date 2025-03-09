#import "AppleUserNotificationCenterApplicationDelegate.h"

#import "Environment/iOS/iOSDetail.h"

@implementation AppleUserNotificationCenterApplicationDelegate

+ (instancetype) sharedInstance {
    static AppleUserNotificationCenterApplicationDelegate *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleUserNotificationCenterApplicationDelegate class]];
    });
    return sharedInstance;
}

#pragma mark - UIApplicationDelegate Protocol

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    UNUserNotificationCenter * center = [UNUserNotificationCenter currentNotificationCenter];
    
    center.delegate = self;
    
    return YES;
}

#pragma mark - UNUserNotificationCenterDelegate Protocol

- (void)userNotificationCenter:(UNUserNotificationCenter *)center willPresentNotification:(UNNotification *)notification withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler API_AVAILABLE(macos(10.14), ios(10.0), watchos(3.0), tvos(10.0)); {
    
    NSLog(@"userNotificationCenter willPresentNotification identifier: %@ title: %@ body: %@"
        , notification.request.identifier
        , notification.request.content.title
        , notification.request.content.body
    );
    
    completionHandler(UNNotificationPresentationOptionAlert | UNNotificationPresentationOptionSound | UNNotificationPresentationOptionBadge);
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
