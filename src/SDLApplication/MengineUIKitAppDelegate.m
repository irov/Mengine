#import "MengineUIKitAppDelegate.h"

#import "../../src/Interface/UIKitProxyApplicationDelegateInterface.h"
@implementation MengineUIKitDelegate

- (id)init {
    NSArray *proxysClassed = [[NSArray alloc] initWithObjects:@"IOSMarSDKNative", nil];
    self.listApplicationDelegateInterfaces = [NSMutableArray array];
    
    for (id className in proxysClassed) {
        Class class =  NSClassFromString(className);
        if(class != nil){
            [self.listApplicationDelegateInterfaces addObject: class];
        }
    }
    
    return [super init];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions API_AVAILABLE(ios(3.0)){
    [super application:application didFinishLaunchingWithOptions:launchOptions];
    
    for(id element in self.listApplicationDelegateInterfaces){
//                [(NSObject<UIKitProxyApplicationDelegateInterface>*)element application:application didFinishLaunchingWithOptions:launchOptions];
        [element application:application didFinishLaunchingWithOptions:launchOptions];
    }
}

//- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken API_AVAILABLE(ios(3.0)){
//    [super  application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
//
//    for(id element in self.listApplicationDelegateInterfaces){
//        [element application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
//    }
//}
//
//- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo API_DEPRECATED("Use UserNotifications Framework's -[UNUserNotificationCenterDelegate willPresentNotification:withCompletionHandler:] or -[UNUserNotificationCenterDelegate didReceiveNotificationResponse:withCompletionHandler:] for user visible notifications and -[UIApplicationDelegate application:didReceiveRemoteNotification:fetchCompletionHandler:] for silent remote notifications", ios(3.0, 10.0)){
//    [super application:application didReceiveRemoteNotification:userInfo];
//
//    for(id element in self.listApplicationDelegateInterfaces){
//        [element application:application didReceiveRemoteNotification:userInfo];
//    }
//}
//
//
//- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult result))completionHandler API_AVAILABLE(ios(7.0)){
//    [super application:application didReceiveRemoteNotification:userInfo fetchCompletionHandler:completionHandler];
//
//    for(id element in self.listApplicationDelegateInterfaces){
//        [element application:application didReceiveRemoteNotification:userInfo];
//    }
//}
//
//
//- (void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification API_DEPRECATED("Use UserNotifications Framework's -[UNUserNotificationCenterDelegate willPresentNotification:withCompletionHandler:] or -[UNUserNotificationCenterDelegate didReceiveNotificationResponse:withCompletionHandler:]", ios(4.0, 10.0)) API_UNAVAILABLE(tvos){
//    [super application:application didReceiveLocalNotification:notification];
//
//    for(id element in self.listApplicationDelegateInterfaces){
//        [element application:application didReceiveLocalNotification:notification];
//    }
//}

- (void)applicationDidBecomeActive:(UIApplication *)application{
    [super applicationDidBecomeActive:application];
    
    for(id element in self.listApplicationDelegateInterfaces){
        [element applicationDidBecomeActive:application ];
    }
}

- (void)applicationWillEnterForeground:(UIApplication *)application{
    [super applicationWillEnterForeground:application];
    
    for(id element in self.listApplicationDelegateInterfaces){
        [element applicationWillEnterForeground:application];
    }
}

- (void)applicationDidEnterBackground:(UIApplication *)application{
    [super applicationDidEnterBackground:application];
    
    for(id element in self.listApplicationDelegateInterfaces){
        [element applicationDidEnterBackground:application];
    }
}

- (void)applicationWillResignActive:(UIApplication *)application{
    [super applicationWillResignActive:application];
    
    for(id element in self.listApplicationDelegateInterfaces){
        [element applicationWillResignActive:application];
    }
}

- (void)applicationWillTerminate:(UIApplication *)application{
    [super applicationWillTerminate:application];
    
    for(id element in self.listApplicationDelegateInterfaces){
        [element applicationWillTerminate:application];
    }
}

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url API_DEPRECATED_WITH_REPLACEMENT("application:openURL:options:", ios(2.0, 9.0)) API_UNAVAILABLE(tvos){
    [super application:application handleOpenURL:url];
    
    for(id element in self.listApplicationDelegateInterfaces){
        [element application:application handleOpenURL:url];
    }
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(nullable NSString *)sourceApplication annotation:(id)annotation API_DEPRECATED_WITH_REPLACEMENT("application:openURL:options:", ios(4.2, 9.0)) API_UNAVAILABLE(tvos){
    [super application:application openURL:url sourceApplication:sourceApplication annotation:annotation];
    
    for(id element in self.listApplicationDelegateInterfaces){
        [element application:application openURL:url sourceApplication:sourceApplication annotation:annotation];
    }
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<UIApplicationOpenURLOptionsKey, id> *)options API_AVAILABLE(ios(9.0)){
    [super application:application openURL:url options:options];
    
    for(id element in self.listApplicationDelegateInterfaces){
        [element application:application openURL:url options:options];
    }
}

@end
