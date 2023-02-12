#import "AppleFirebaseMessagingApplicationDelegate.h"

#include "AppleFirebaseMessagingInterface.h"

#import <Firebase/Firebase.h>

@implementation AppleFirebaseMessagingApplicationDelegate

#pragma mark - UIKitProxyApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [[FIRMessaging messaging] setDelegate:self];
}

- (void)messaging:(FIRMessaging *)messaging didReceiveRegistrationToken:(NSString *)fcmToken {
    NSDictionary *dataDict = [NSDictionary dictionaryWithObject:fcmToken forKey:@"token"];
    [[NSNotificationCenter defaultCenter] postNotificationName:@"FCMToken" object:nil userInfo:dataDict];
    
    APPLE_FIREBASE_MESSAGING_SERVICE()
        ->setPushToken( fcmToken );
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    [FIRMessaging messaging].APNSToken = deviceToken;
}

// iOS 7+
- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(nonnull void (^)(UIBackgroundFetchResult))completionHandler {

    [[FIRMessaging messaging] appDidReceiveMessage:userInfo];

    completionHandler(UIBackgroundFetchResultNoData);
}

- (void)applicationWillResignActive:(UIApplication *)application {
    //Empty
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    //Empty
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    //Empty
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    //Empty
}

- (void)applicationWillTerminate:(UIApplication *)application {
    //Empty
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo {
    //Empty
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation handled:(BOOL *)handler {
    //Empty
    
    return NO;
}
    
- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary *)options handled:(BOOL *)handler {
    //Empty
    
    return NO;
}

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url handled:(BOOL *)handler {
    //Empty
    
    return NO;
}


@end
