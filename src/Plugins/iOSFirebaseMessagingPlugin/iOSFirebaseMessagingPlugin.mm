#import "iOSFirebaseMessagingPlugin.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

@implementation iOSFirebaseMessagingPlugin

+ (instancetype)sharedInstance {
    static iOSFirebaseMessagingPlugin * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[iOSFirebaseMessagingPlugin class]];
    });
    return sharedInstance;
}

#pragma mark - UIApplicationDelegate Protocol

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    MENGINE_UNUSED(application);
    MENGINE_UNUSED(launchOptions);

    [[FIRMessaging messaging] setDelegate:self];

    return YES;
}

#pragma mark - FIRMessagingDelegate Protocol

- (void)messaging:(FIRMessaging *)messaging didReceiveRegistrationToken:(NSString *)fcmToken {
    MENGINE_UNUSED(messaging);

    if (fcmToken == nil) {
        IOS_LOGGER_WARNING(@"iOS Firebase Messaging received an empty registration token");

        self.m_pushToken = nil;

        return;
    }

    self.m_pushToken = fcmToken;

    NSDictionary *dataDict = [NSDictionary dictionaryWithObject:fcmToken forKey:@"token"];
    [[NSNotificationCenter defaultCenter] postNotificationName:@"FCMToken" object:nil userInfo:dataDict];
}

#pragma mark - UISceneSession lifecycle

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    MENGINE_UNUSED(application);

    [FIRMessaging messaging].APNSToken = deviceToken;
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(nonnull void (^)(UIBackgroundFetchResult))completionHandler {
    MENGINE_UNUSED(application);

    [[FIRMessaging messaging] appDidReceiveMessage:userInfo];

    completionHandler(UIBackgroundFetchResultNoData);
}

#pragma mark - iOSFirebaseMessagingInterface

- (NSString *)getPushToken {
    return self.m_pushToken;
}

@end
