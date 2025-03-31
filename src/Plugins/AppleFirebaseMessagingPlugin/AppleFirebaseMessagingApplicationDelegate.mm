#import "AppleFirebaseMessagingApplicationDelegate.h"

#include "AppleFirebaseMessagingInterface.h"

#import <Firebase/Firebase.h>

@implementation AppleFirebaseMessagingApplicationDelegate

#pragma mark - UIApplicationDelegate Protocol

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [[FIRMessaging messaging] setDelegate:self];
    
    return YES;
}

#pragma mark - FIRMessagingDelegate Protocol

- (void)messaging:(FIRMessaging *)messaging didReceiveRegistrationToken:(NSString *)fcmToken {
    NSDictionary *dataDict = [NSDictionary dictionaryWithObject:fcmToken forKey:@"token"];
    [[NSNotificationCenter defaultCenter] postNotificationName:@"FCMToken" object:nil userInfo:dataDict];
    
    APPLE_FIREBASE_MESSAGING_SERVICE()
        ->setPushToken( fcmToken );
}

#pragma mark - UISceneSession lifecycle

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    [FIRMessaging messaging].APNSToken = deviceToken;
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(nonnull void (^)(UIBackgroundFetchResult))completionHandler {

    [[FIRMessaging messaging] appDidReceiveMessage:userInfo];

    completionHandler(UIBackgroundFetchResultNoData);
}

@end
