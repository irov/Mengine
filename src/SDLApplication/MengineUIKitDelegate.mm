#import "MengineUIKitDelegate.h"

#import "MengineAppleApplicationDelegates.h"

#import "Environment/iOS/UIKitProxyApplicationDelegateInterface.h"

@implementation MengineUIKitDelegate

- (id)init {
    NSArray * proxysClassed = getMengineAppleApplicationDelegates();

    self.m_applicationDelegates = [NSMutableArray array];
    
    for (id className in proxysClassed) {
        id c = NSClassFromString(className);

        if (c == nil) {
            continue;
        }
        
        id delegate = [[c alloc] init];

        [self.m_applicationDelegates addObject:delegate];
    }
    
    return [super init];
}

- (void)dealloc {
    self.m_applicationDelegates = nil;
    
    [super dealloc];
}

#pragma mark - UIApplicationDelegate Protocol

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions API_AVAILABLE(ios(3.0)) {
    for (id delegate in self.m_applicationDelegates) {
        if ([delegate application:application didFinishLaunchingWithOptions:launchOptions] == NO) {
            return NO;
        }
    }
    
    if ([super application:application didFinishLaunchingWithOptions:launchOptions] == NO) {
        return NO;
    }
    
    return YES;
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken API_AVAILABLE(ios(3.0)) {
    for (id delegate in self.m_applicationDelegates) {
        if ([delegate respondsToSelector:@selector(application: didRegisterForRemoteNotificationsWithDeviceToken:)] == YES) {
            [delegate application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
        }
    }
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult result))completionHandler API_AVAILABLE(ios(7.0)) {

    for (id delegate in self.m_applicationDelegates) {
        if ([delegate respondsToSelector:@selector(application: didReceiveRemoteNotification: fetchCompletionHandler:)] == YES) {
            [delegate application:application didReceiveRemoteNotification:userInfo fetchCompletionHandler:completionHandler];
        }
    }
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    for (id delegate in self.m_applicationDelegates) {
        if ([delegate respondsToSelector:@selector(applicationDidBecomeActive:)] == YES) {
            [delegate applicationDidBecomeActive:application];
        }
    }
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    for (id delegate in self.m_applicationDelegates) {
        if ([delegate respondsToSelector:@selector(applicationWillEnterForeground:)] == YES) {
            [delegate applicationWillEnterForeground:application];
        }
    }
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    for (id delegate in self.m_applicationDelegates) {
        if ([delegate respondsToSelector:@selector(applicationDidEnterBackground:)] == YES) {
            [delegate applicationDidEnterBackground:application];
        }
    }
}

- (void)applicationWillResignActive:(UIApplication *)application {
    for (id delegate in self.m_applicationDelegates) {
        if ([delegate respondsToSelector:@selector(applicationWillResignActive:)] == YES) {
            [delegate applicationWillResignActive:application];
        }
    }
}

- (void)applicationWillTerminate:(UIApplication *)application {
    for (id delegate in self.m_applicationDelegates) {
        if ([delegate respondsToSelector:@selector(applicationWillTerminate:)] == YES) {
            [delegate applicationWillTerminate:application];
        }
    }
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<UIApplicationOpenURLOptionsKey, id> *)options API_AVAILABLE(ios(9.0)) {
    for (id delegate in self.m_applicationDelegates) {
        if ([delegate respondsToSelector:@selector(application: openURL: options: handled:)] == YES) {
            BOOL handler = NO;
            BOOL result = [delegate application:application openURL:url options:options handled:&handler];
            
            if (handler == YES) {
                return result;
            }
        }
    }
    
    return [super application:application openURL:url options:options];
}

@end
