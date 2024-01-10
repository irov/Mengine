#import "SDLUIApplicationDelegate.h"

#import "MengineAppleApplicationDelegates.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/SDLPlatformServiceExtensionInterface.h"

#include "Environment/SDL/SDLIncluder.h"

int MENGINE_MAIN_argc = 0;
char ** MENGINE_MAIN_argv = nullptr;
Mengine::SDLApplication MENGINE_application;

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
    NSLog(@"Mengine application didFinishLaunchingWithOptions");
    
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate application:application didFinishLaunchingWithOptions:launchOptions] == NO) {
            NSLog(@"Mengine application didFinishLaunchingWithOptions plugin %@ failed", NSStringFromClass([delegate class]));
            
            return NO;
        }
    }
    
    SDL_SetMainReady();
    
    SDL_iPhoneSetEventPump( SDL_TRUE );
    
    NSLog(@"Mengine application initialize");
    
    if( MENGINE_application.bootstrap( MENGINE_MAIN_argc, MENGINE_MAIN_argv ) == false ) {
        NSLog(@"Mengine application bootstrap [Failed]");
        
        NSLog(@"Mengine application begin finalize");
        
        MENGINE_application.finalize();
        
        NSLog(@"Mengine application end finalize");
        
        SDL_iPhoneSetEventPump( SDL_FALSE );
        
        return NO;
    }
    
    NSLog(@"Mengine application initialize [Successful]");

    if( MENGINE_application.initialize() == false ) {
        NSLog(@"Mengine application initialize [Failed]");
        
        NSLog(@"Mengine application begin finalize");
        
        MENGINE_application.finalize();
        
        NSLog(@"Mengine application end finalize");
        
        SDL_iPhoneSetEventPump( SDL_FALSE );
        
        return NO;
    }
    
    NSLog(@"Mengine application initialize [Successful]");
    
    [self performSelector:@selector(postFinishLaunch) withObject:nil afterDelay:0.0];
    
    return YES;
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken API_AVAILABLE(ios(3.0)) {
    for (id delegate in self.m_pluginDelegates) {
        NSLog(@"Mengine application didRegisterForRemoteNotificationsWithDeviceToken");
        
        if ([delegate respondsToSelector:@selector(application: didRegisterForRemoteNotificationsWithDeviceToken:)] == NO) {
            continue;
        }
        
        [delegate application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
    }
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult result))completionHandler API_AVAILABLE(ios(7.0)) {
    NSLog(@"Mengine application didReceiveRemoteNotification");
    
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(application: didReceiveRemoteNotification: fetchCompletionHandler:)] == NO) {
            continue;
        }
        
        [delegate application:application didReceiveRemoteNotification:userInfo fetchCompletionHandler:completionHandler];
    }
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    NSLog(@"Mengine application applicationDidBecomeActive");
    
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(applicationDidBecomeActive:)] == NO) {
            continue;
        }
        
        [delegate applicationDidBecomeActive:application];
    }
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    NSLog(@"Mengine application applicationWillEnterForeground");
    
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(applicationWillEnterForeground:)] == NO) {
            continue;
        }
        
        [delegate applicationWillEnterForeground:application];
    }
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    NSLog(@"Mengine application applicationDidEnterBackground");
    
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(applicationDidEnterBackground:)] == NO) {
            continue;
        }
        
        [delegate applicationDidEnterBackground:application];
    }
}

- (void)applicationWillResignActive:(UIApplication *)application {
    NSLog(@"Mengine application applicationWillResignActive");
    
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(applicationWillResignActive:)] == NO) {
            continue;
        }
        
        [delegate applicationWillResignActive:application];
    }
}

- (void)applicationWillTerminate:(UIApplication *)application {
    NSLog(@"Mengine application applicationWillTerminate");
    
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(applicationWillTerminate:)] == NO) {
            continue;
        }
        
        [delegate applicationWillTerminate:application];
    }
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<UIApplicationOpenURLOptionsKey, id> *)options API_AVAILABLE(ios(9.0)) {
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(application: openURL: options: handled:)] == NO) {
            continue;
        }
        
        BOOL handler = NO;
        BOOL result = [delegate application:application openURL:url options:options handled:&handler];
        
        if (handler == YES) {
            return result;
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
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(beginLoop:)] == NO) {
            continue;
        }
         
        [delegate beginLoop];
    }
    
    NSLog(@"Mengine application start loop");
    
    MENGINE_application.loop();
    
    NSLog(@"Mengine application end loop");
    
    NSLog(@"Mengine application begin finalize");
    
    MENGINE_application.finalize();
    
    NSLog(@"Mengine application end finalize");
    
    SDL_iPhoneSetEventPump( SDL_FALSE );
    
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(endLoop:)] == NO) {
            continue;
        }
         
        [delegate endLoop];
    }
}

@end
