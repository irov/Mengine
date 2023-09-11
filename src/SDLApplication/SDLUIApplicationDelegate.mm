#import "SDLUIApplicationDelegate.h"

#import "MengineAppleApplicationDelegates.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/SDLPlatformServiceExtensionInterface.h"

#include "Environment/SDL/SDLIncluder.h"

@implementation SDLUIApplicationDelegate

- (id)init {
    NSArray * proxysClassed = getMengineAppleApplicationDelegates();

    self.m_pluginDelegates = [NSMutableArray<UIPluginApplicationDelegateInterface> array];
    
    for (id className in proxysClassed) {
        id c = NSClassFromString(className);

        if (c == nil) {
            continue;
        }
        
        id delegate = [[c alloc] init];

        [self.m_pluginDelegates addObject:delegate];
    }
    
    return [super init];
}

- (void)dealloc {
    self.m_pluginDelegates = nil;
}

#pragma mark - UIMainApplicationDelegateInterface Protocol

- (NSArray<UIPluginApplicationDelegateInterface> *)getPluginDelegates {
    return self.m_pluginDelegates;
}

- (void)notify:(NSString *)name args:(id)firstArg, ... NS_REQUIRES_NIL_TERMINATION {
    va_list args;
    va_start(args, firstArg);
    
    NSMutableArray * send_args = [[NSMutableArray alloc] init];

    for( NSString *arg = firstArg; arg != nil; arg = va_arg(args, NSString*) ) {
        [send_args addObject:firstArg];
    }

    va_end(args);
    
    [self notify:name arrayArgs:send_args];
}

- (void)notify:(NSString *)name arrayArgs:(NSArray<id> *)args {
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(event: args:)] == YES) {
            [delegate event:name args:args];
        }
    }
}

#pragma mark - UIApplicationDelegate Protocol

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions API_AVAILABLE(ios(3.0)) {
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate application:application didFinishLaunchingWithOptions:launchOptions] == NO) {
            return NO;
        }
    }
    
    SDL_SetMainReady();
    
    SDL_iPhoneSetEventPump(SDL_TRUE);
    
    bool initialize = self.m_application.initialize( 0, nullptr );

    if( initialize == false ) {
        return NO;
    }
    
    [self performSelector:@selector(postFinishLaunch) withObject:self afterDelay:0.0];
    
    return YES;
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken API_AVAILABLE(ios(3.0)) {
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(application: didRegisterForRemoteNotificationsWithDeviceToken:)] == YES) {
            [delegate application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
        }
    }
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult result))completionHandler API_AVAILABLE(ios(7.0)) {
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(application: didReceiveRemoteNotification: fetchCompletionHandler:)] == YES) {
            [delegate application:application didReceiveRemoteNotification:userInfo fetchCompletionHandler:completionHandler];
        }
    }
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(applicationDidBecomeActive:)] == YES) {
            [delegate applicationDidBecomeActive:application];
        }
    }
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(applicationWillEnterForeground:)] == YES) {
            [delegate applicationWillEnterForeground:application];
        }
    }
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(applicationDidEnterBackground:)] == YES) {
            [delegate applicationDidEnterBackground:application];
        }
    }
}

- (void)applicationWillResignActive:(UIApplication *)application {
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(applicationWillResignActive:)] == YES) {
            [delegate applicationWillResignActive:application];
        }
    }
}

- (void)applicationWillTerminate:(UIApplication *)application {
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(applicationWillTerminate:)] == YES) {
            [delegate applicationWillTerminate:application];
        }
    }
    
    self.m_application.finalize();
    
    SDL_iPhoneSetEventPump(SDL_FALSE);
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<UIApplicationOpenURLOptionsKey, id> *)options API_AVAILABLE(ios(9.0)) {
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(application: openURL: options: handled:)] == YES) {
            BOOL handler = NO;
            BOOL result = [delegate application:application openURL:url options:options handled:&handler];
            
            if (handler == YES) {
                return result;
            }
        }
    }
    
    return NO;
}

- (UIWindow *)window {
    if( SERVICE_PROVIDER_EXIST() == false ) {
        return nil;
    }
    
    if( SERVICE_IS_INITIALIZE(Mengine::PlatformServiceInterface) == false ) {
        return nil;
    }
    
    Mengine::SDLPlatformServiceExtensionInterface * sdlPlatform = PLATFORM_SERVICE()
        ->getDynamicUnknown();
    
    UIWindow * window = sdlPlatform->getUIWindow();
     
    return window;
}

- (void)setWindow:(UIWindow *)window {
    /* Do nothing. */
}

- (void)postFinishLaunch {
    self.m_application.loop();
}

@end
