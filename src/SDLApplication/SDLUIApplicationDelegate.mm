#import "SDLUIApplicationDelegate.h"

#import "MengineAppleApplicationDelegates.h"

#include "Interface/PlatformServiceInterface.h"

#include "Environment/SDL/SDLIncluder.h"
#include "Environment/SDL/SDLPlatformServiceExtensionInterface.h"
#include "Environment/Apple/AppleUserDefaults.h"
#include "Environment/Apple/AppleDetail.h"
#include "Environment/Apple/AppleLog.h"

int MENGINE_MAIN_argc = 0;
char ** MENGINE_MAIN_argv = nullptr;

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
    Mengine::Helper::AppleLog(@"Mengine application didFinishLaunchingWithOptions");
    
    NSString * install_key = Mengine::Helper::AppleGetUserDefaultsString(@"mengine.install_key", nil);
    NSInteger install_timestamp = Mengine::Helper::AppleGetUserDefaultsInteger(@"mengine.install_timestamp", -1);
    NSString * install_version = Mengine::Helper::AppleGetUserDefaultsString(@"mengine.install_version", nil);
    NSInteger install_rnd = Mengine::Helper::AppleGetUserDefaultsInteger(@"mengine.install_rnd", -1);
    NSInteger session_index = Mengine::Helper::AppleGetUserDefaultsInteger(@"mengine.session_index", 0);
    NSString * session_id = Mengine::Helper::AppleGetUserDefaultsString(@"mengine.session_id", nil);
    
    if (install_key == nil) {
        install_key = [@"MNIK" stringByAppendingString:Mengine::Helper::AppleGetRandomHexString(16)];
        install_timestamp = Mengine::Helper::AppleCurrentTimeMillis();
        install_version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
        install_rnd = Mengine::Helper::AppleGetSecureRandomInteger();
        
        if (install_rnd == 0) {
            install_rnd = 1;
        } else if (install_rnd < 0) {
            install_rnd = -install_rnd;
        }
        
        Mengine::Helper::AppleSetUserDefaultsString(@"mengine.install_key", install_key);
        Mengine::Helper::AppleSetUserDefaultsInteger(@"mengine.install_timestamp", install_timestamp);
        Mengine::Helper::AppleSetUserDefaultsString(@"mengine.install_version", install_version);
        Mengine::Helper::AppleSetUserDefaultsInteger(@"mengine.install_rnd", install_rnd);
    }
    
    if (session_id == nil) {
        session_id = install_key;
        
        Mengine::Helper::AppleSetUserDefaultsString(@"mengine.session_id", session_id);
    }
    
    Mengine::Helper::AppleSetUserDefaultsInteger(@"mengine.session_index", session_index + 1);
    
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate application:application didFinishLaunchingWithOptions:launchOptions] == NO) {
            Mengine::Helper::AppleLog(@"🔴 [ERROR] Mengine application didFinishLaunchingWithOptions plugin %@ failed", NSStringFromClass([delegate class]));
            
            return NO;
        }
    }
    
    SDL_SetMainReady();
    
    SDL_iPhoneSetEventPump( SDL_TRUE );
    
    [self performSelector:@selector(postFinishLaunch) withObject:nil afterDelay:0.0];
    
    return YES;
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken API_AVAILABLE(ios(3.0)) {
    for (id delegate in self.m_pluginDelegates) {
        Mengine::Helper::AppleLog(@"Mengine application didRegisterForRemoteNotificationsWithDeviceToken");
        
        if ([delegate respondsToSelector:@selector(application: didRegisterForRemoteNotificationsWithDeviceToken:)] == NO) {
            continue;
        }
        
        [delegate application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
    }
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult result))completionHandler API_AVAILABLE(ios(7.0)) {
    Mengine::Helper::AppleLog(@"Mengine application didReceiveRemoteNotification");
    
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(application: didReceiveRemoteNotification: fetchCompletionHandler:)] == NO) {
            continue;
        }
        
        [delegate application:application didReceiveRemoteNotification:userInfo fetchCompletionHandler:completionHandler];
    }
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    Mengine::Helper::AppleLog(@"Mengine application applicationDidBecomeActive");
    
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(applicationDidBecomeActive:)] == NO) {
            continue;
        }
        
        [delegate applicationDidBecomeActive:application];
    }
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    Mengine::Helper::AppleLog(@"Mengine application applicationWillEnterForeground");
    
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(applicationWillEnterForeground:)] == NO) {
            continue;
        }
        
        [delegate applicationWillEnterForeground:application];
    }
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    Mengine::Helper::AppleLog(@"Mengine application applicationDidEnterBackground");
    
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(applicationDidEnterBackground:)] == NO) {
            continue;
        }
        
        [delegate applicationDidEnterBackground:application];
    }
}

- (void)applicationWillResignActive:(UIApplication *)application {
    Mengine::Helper::AppleLog(@"Mengine application applicationWillResignActive");
    
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(applicationWillResignActive:)] == NO) {
            continue;
        }
        
        [delegate applicationWillResignActive:application];
    }
}

- (void)applicationWillTerminate:(UIApplication *)application {
    Mengine::Helper::AppleLog(@"Mengine application applicationWillTerminate");
    
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
    
    Mengine::SDLApplication application;
        
    if( application.bootstrap( MENGINE_MAIN_argc, MENGINE_MAIN_argv ) == false ) {
        Mengine::Helper::AppleLog(@"🔴 [ERROR] Mengine application bootstrap [Failed]");
        
        application.finalize();
        
        SDL_iPhoneSetEventPump( SDL_FALSE );
        
        return NO;
    }

    if( application.initialize() == false ) {
        Mengine::Helper::AppleLog(@"🔴 [ERROR] Mengine application initialize [Failed]");
                
        application.finalize();
        
        SDL_iPhoneSetEventPump( SDL_FALSE );
        
        return NO;
    }
    
    application.loop();
    
    application.finalize();
    
    SDL_iPhoneSetEventPump( SDL_FALSE );
    
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate respondsToSelector:@selector(endLoop:)] == NO) {
            continue;
        }
         
        [delegate endLoop];
    }
    
    Mengine::Helper::AppleLog(@"Mengine application finish");
}

@end
