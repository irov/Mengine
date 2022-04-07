#import "MengineUIKitAppDelegate.h"

#import "Interface/UIKitProxyApplicationDelegateInterface.h"

//////////////////////////////////////////////////////////////////////////
@implementation MengineUIKitDelegate
//////////////////////////////////////////////////////////////////////////
- (id)init {
    NSArray *proxysClassed = [[NSArray alloc] initWithObjects:@"AppleMARSDKApplicationDelegate", nil];

    self.m_applicationDelegates = [NSMutableArray array];
    
    for (id className in proxysClassed) {
        Class class = NSClassFromString(className);

        if( class == nil ) {
            continue;
        }

        [self.m_applicationDelegates addObject: class];
    }
    
    return [super init];
}
//////////////////////////////////////////////////////////////////////////
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions API_AVAILABLE(ios(3.0)) {
    
    for(id delegate in self.m_applicationDelegates) {
        [delegate application:application didFinishLaunchingWithOptions:launchOptions];
    }
    
    return [super application:application didFinishLaunchingWithOptions:launchOptions];
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken API_AVAILABLE(ios(3.0)){

    for(id delegate in self.m_applicationDelegates){
        [delegate application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
    }
    [super  application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo API_DEPRECATED("Use UserNotifications Framework's -[UNUserNotificationCenterDelegate willPresentNotification:withCompletionHandler:] or -[UNUserNotificationCenterDelegate didReceiveNotificationResponse:withCompletionHandler:] for user visible notifications and -[UIApplicationDelegate application:didReceiveRemoteNotification:fetchCompletionHandler:] for silent remote notifications", ios(3.0, 10.0)){

    for(id delegate in self.m_applicationDelegates){
        [delegate application:application didReceiveRemoteNotification:userInfo];
    }
    [super application:application didReceiveRemoteNotification:userInfo];
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult result))completionHandler API_AVAILABLE(ios(7.0)){

    for(id delegate in self.m_applicationDelegates){
        [delegate application:application didReceiveRemoteNotification:userInfo];
    }
    [super application:application didReceiveRemoteNotification:userInfo fetchCompletionHandler:completionHandler];
}

- (void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification API_DEPRECATED("Use UserNotifications Framework's -[UNUserNotificationCenterDelegate willPresentNotification:withCompletionHandler:] or -[UNUserNotificationCenterDelegate didReceiveNotificationResponse:withCompletionHandler:]", ios(4.0, 10.0)) API_UNAVAILABLE(tvos){

    for(id delegate in self.m_applicationDelegates){
        [delegate application:application didReceiveLocalNotification:notification];
    }
    [super application:application didReceiveLocalNotification:notification];
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    
    for(id delegate in self.m_applicationDelegates) {
        [delegate applicationDidBecomeActive:application ];
    }
    
    [super applicationDidBecomeActive:application];
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    [super applicationWillEnterForeground:application];
    
    for(id delegate in self.m_applicationDelegates) {
        [delegate applicationWillEnterForeground:application];
    }
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    [super applicationDidEnterBackground:application];
    
    for(id delegate in self.m_applicationDelegates) {
        [delegate applicationDidEnterBackground:application];
    }
}

- (void)applicationWillResignActive:(UIApplication *)application {
    
    for(id delegate in self.m_applicationDelegates) {
        [delegate applicationWillResignActive:application];
    }
    
    [super applicationWillResignActive:application];
}

- (void)applicationWillTerminate:(UIApplication *)application {
    
    for(id delegate in self.m_applicationDelegates) {
        [delegate applicationWillTerminate:application];
    }
    
    [super applicationWillTerminate:application];
}

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url API_DEPRECATED_WITH_REPLACEMENT("application:openURL:options:", ios(2.0, 9.0)) API_UNAVAILABLE(tvos) {
    
    for(id delegate in self.m_applicationDelegates) {
        [delegate application:application handleOpenURL:url];
    }
    
    return [super application:application handleOpenURL:url];
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(nullable NSString *)sourceApplication annotation:(id)annotation API_DEPRECATED_WITH_REPLACEMENT("application:openURL:options:", ios(4.2, 9.0)) API_UNAVAILABLE(tvos) {
    
    for(id delegate in self.m_applicationDelegates) {
        [delegate application:application openURL:url sourceApplication:sourceApplication annotation:annotation];
    }
    
    return [super application:application openURL:url sourceApplication:sourceApplication annotation:annotation];
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<UIApplicationOpenURLOptionsKey, id> *)options API_AVAILABLE(ios(9.0)) {
    
    for(id delegate in self.m_applicationDelegates) {
        [delegate application:application openURL:url options:options];
    }
    
    return [super application:application openURL:url options:options];
}

@end
