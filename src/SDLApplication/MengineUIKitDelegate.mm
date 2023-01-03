#import "MengineUIKitDelegate.h"

#import "MengineAppleApplicationDelegates.h"

#import "Interface/UIKitProxyApplicationDelegateInterface.h"

//////////////////////////////////////////////////////////////////////////
@implementation MengineUIKitDelegate
//////////////////////////////////////////////////////////////////////////
- (id)init {
    NSArray *proxysClassed = getMengineAppleApplicationDelegates();

    self.m_applicationDelegates = [NSMutableArray array];
    
    for (id className in proxysClassed) {
        id c = NSClassFromString(className);

        if( c == nil ) {
            continue;
        }
        
        id delegate = [c alloc];

        [self.m_applicationDelegates addObject: delegate];
    }
    
    return [super init];
}
//////////////////////////////////////////////////////////////////////////
- (void)dealloc {
    [self.m_applicationDelegates release];
    self.m_applicationDelegates = nil;
    
    [super dealloc];
}
//////////////////////////////////////////////////////////////////////////
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions API_AVAILABLE(ios(3.0)) {
    for (id delegate in self.m_applicationDelegates) {
        if ([delegate application:application didFinishLaunchingWithOptions:launchOptions] == NO) {
            return NO;
        }
    }
    
    return [super application:application didFinishLaunchingWithOptions:launchOptions];
}
//////////////////////////////////////////////////////////////////////////
- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken API_AVAILABLE(ios(3.0)) {
    for (id delegate in self.m_applicationDelegates) {
        [delegate application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
    }
}
//////////////////////////////////////////////////////////////////////////
- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo API_DEPRECATED("Use UserNotifications Framework's -[UNUserNotificationCenterDelegate willPresentNotification:withCompletionHandler:] or -[UNUserNotificationCenterDelegate didReceiveNotificationResponse:withCompletionHandler:] for user visible notifications and -[UIApplicationDelegate application:didReceiveRemoteNotification:fetchCompletionHandler:] for silent remote notifications", ios(3.0, 10.0)) {
    for (id delegate in self.m_applicationDelegates) {
        [delegate application:application didReceiveRemoteNotification:userInfo];
    }
}
//////////////////////////////////////////////////////////////////////////
- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult result))completionHandler API_AVAILABLE(ios(7.0)) {

    for (id delegate in self.m_applicationDelegates) {
        [delegate application:application didReceiveRemoteNotification:userInfo fetchCompletionHandler:completionHandler];
    }
}
//////////////////////////////////////////////////////////////////////////
- (void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification API_DEPRECATED("Use UserNotifications Framework's -[UNUserNotificationCenterDelegate willPresentNotification:withCompletionHandler:] or -[UNUserNotificationCenterDelegate didReceiveNotificationResponse:withCompletionHandler:]", ios(4.0, 10.0)) API_UNAVAILABLE(tvos) {
    for(id delegate in self.m_applicationDelegates) {
        [delegate application:application didReceiveLocalNotification:notification];
    }
}
//////////////////////////////////////////////////////////////////////////
- (void)applicationDidBecomeActive:(UIApplication *)application {
    for (id delegate in self.m_applicationDelegates) {
        [delegate applicationDidBecomeActive:application];
    }
}
//////////////////////////////////////////////////////////////////////////
- (void)applicationWillEnterForeground:(UIApplication *)application {
    for (id delegate in self.m_applicationDelegates) {
        [delegate applicationWillEnterForeground:application];
    }
}
//////////////////////////////////////////////////////////////////////////
- (void)applicationDidEnterBackground:(UIApplication *)application {
    for(id delegate in self.m_applicationDelegates) {
        [delegate applicationDidEnterBackground:application];
    }
}
//////////////////////////////////////////////////////////////////////////
- (void)applicationWillResignActive:(UIApplication *)application {
    for(id delegate in self.m_applicationDelegates) {
        [delegate applicationWillResignActive:application];
    }
}
//////////////////////////////////////////////////////////////////////////
- (void)applicationWillTerminate:(UIApplication *)application {
    for (id delegate in self.m_applicationDelegates) {
        [delegate applicationWillTerminate:application];
    }
}
//////////////////////////////////////////////////////////////////////////
- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url API_DEPRECATED_WITH_REPLACEMENT("application:openURL:options:", ios(2.0, 9.0)) API_UNAVAILABLE(tvos) {
    for (id delegate in self.m_applicationDelegates) {
        BOOL handler = NO;
        BOOL result = [delegate application:application handleOpenURL:url handled:&handler];
        
        if (handler == YES) {
            return result;
        }
    }
    
    return NO;
}
//////////////////////////////////////////////////////////////////////////
- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(nullable NSString *)sourceApplication annotation:(id)annotation API_DEPRECATED_WITH_REPLACEMENT("application:openURL:options:", ios(4.2, 9.0)) API_UNAVAILABLE(tvos) {
    for(id delegate in self.m_applicationDelegates) {
        BOOL handler = NO;
        BOOL result = [delegate application:application openURL:url sourceApplication:sourceApplication annotation:annotation handled:&handler];
        
        if (handler == YES) {
            return result;
        }
    }
    
    return [super application:application openURL:url sourceApplication:sourceApplication annotation:annotation];
}
//////////////////////////////////////////////////////////////////////////
- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<UIApplicationOpenURLOptionsKey, id> *)options API_AVAILABLE(ios(9.0)) {
    for (id delegate in self.m_applicationDelegates) {
        BOOL handler = NO;
        BOOL result = [delegate application:application openURL:url options:options handled:&handler];
        
        if (handler == YES) {
            return result;
        }
    }
    
    return [super application:application openURL:url options:options];
}
//////////////////////////////////////////////////////////////////////////
@end
