#import "iOSUIApplicationDelegate.h"

#include "Interface/PlatformServiceInterface.h"

#include "Environment/SDL/SDLIncluder.h"
#include "Environment/SDL/SDLPlatformServiceExtensionInterface.h"

#import "iOSApplicationDelegates.h"

#import "Environment/Apple/AppleUserDefaults.h"
#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleLog.h"
#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSNetwork.h"
#import "Environment/iOS/iOSDetail.h"

#include "iOSApplication.h"

@implementation iOSUIApplicationDelegate

- (instancetype)init {
    if (self = [super init]) {
        NSArray * proxysClassed = [iOSApplicationDelegates getApplicationDelegates];
        
        self.m_pluginDelegates = [NSMutableArray<id> array];
        self.m_pluginApplicationDelegates = [NSMutableArray<iOSPluginApplicationDelegateInterface> array];
        self.m_pluginLoggerDelegates = [NSMutableArray<iOSPluginLoggerDelegateInterface> array];
        self.m_pluginConfigDelegates = [NSMutableArray<iOSPluginConfigDelegateInterface> array];
        self.m_pluginSessionIdDelegates = [NSMutableArray<iOSPluginSessionIdDelegateInterface> array];
        self.m_pluginAdRevenueDelegates = [NSMutableArray<iOSPluginAdRevenueDelegateInterface> array];
        self.m_pluginTransparencyConsentDelegates = [NSMutableArray<iOSPluginTransparencyConsentDelegateInterface> array];
        
        for (id className in proxysClassed) {
            id c = NSClassFromString(className);
            
            if (c == nil) {
                continue;
            }
            
            id delegate = [[c alloc] init];
            
            [self.m_pluginDelegates addObject:delegate];
            
            if ([delegate conformsToProtocol:@protocol(iOSPluginApplicationDelegateInterface)] == YES) {
                [self.m_pluginApplicationDelegates addObject:delegate];
            }
            
            if ([delegate conformsToProtocol:@protocol(iOSPluginLoggerDelegateInterface)] == YES) {
                [self.m_pluginLoggerDelegates addObject:delegate];
            }
            
            if ([delegate conformsToProtocol:@protocol(iOSPluginConfigDelegateInterface)] == YES) {
                [self.m_pluginConfigDelegates addObject:delegate];
            }
            
            if ([delegate conformsToProtocol:@protocol(iOSPluginSessionIdDelegateInterface)] == YES) {
                [self.m_pluginSessionIdDelegates addObject:delegate];
            }
            
            if ([delegate conformsToProtocol:@protocol(iOSPluginAdRevenueDelegateInterface)] == YES) {
                [self.m_pluginAdRevenueDelegates addObject:delegate];
            }
            
            if ([delegate conformsToProtocol:@protocol(iOSPluginTransparencyConsentDelegateInterface)] == YES) {
                [self.m_pluginTransparencyConsentDelegates addObject:delegate];
            }
        }
    }
    
    return self;
}

#pragma mark - iOSUIMainApplicationDelegateInterface Protocol

- (NSArray<iOSPluginApplicationDelegateInterface> *)getPluginApplicationDelegates {
    return self.m_pluginApplicationDelegates;
}

- (NSArray<iOSPluginLoggerDelegateInterface> *)getPluginLoggerDelegates {
    return self.m_pluginLoggerDelegates;
}

- (NSArray<iOSPluginConfigDelegateInterface> *)getPluginConfigDelegates {
    return self.m_pluginConfigDelegates;
}

- (NSArray<iOSPluginSessionIdDelegateInterface> *)getPluginSessionIdDelegates {
    return self.m_pluginSessionIdDelegates;
}

- (NSArray<iOSPluginAdRevenueDelegateInterface> *)getPluginAdRevenueDelegates {
    return self.m_pluginAdRevenueDelegates;
}

- (NSArray<iOSPluginTransparencyConsentDelegateInterface> *)getPluginTransparencyConsentDelegates {
    return self.m_pluginTransparencyConsentDelegates;
}

- (id _Nullable)getPluginDelegateOfClass:(Class)delegateClass {
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate isKindOfClass:delegateClass] == NO) {
            continue;
        }
         
        return delegate;
    }
    
    return nil;
}

- (id _Nullable)getPluginDelegateOfProtocol:(Protocol *)protocol {
    for (id delegate in self.m_pluginDelegates) {
        if ([delegate conformsToProtocol:protocol] == NO) {
            continue;
        }
        
        return delegate;
    }
    
    return nil;
}

- (void)notify:(AppleEvent *)event args:(id)firstArg, ... NS_REQUIRES_NIL_TERMINATION {
    va_list args;
    va_start(args, firstArg);
    
    NSMutableArray * send_args = [[NSMutableArray alloc] init];

    for( NSString *arg = firstArg; arg != nil; arg = va_arg(args, NSString*) ) {
        [send_args addObject:firstArg];
    }

    va_end(args);
    
    [self notify:event arrayArgs:send_args];
}

- (void)notify:(AppleEvent *)event arrayArgs:(NSArray<id> *)args {
    @autoreleasepool {
        for (NSObject<iOSPluginApplicationDelegateInterface> * delegate in self.m_pluginApplicationDelegates) {
            if ([delegate respondsToSelector:@selector(onEvent: args:)] == YES) {
                [delegate onEvent:event args:args];
            }
        }
    }
}

- (void)log:(AppleLogRecordParam *)record {
    @autoreleasepool {
        for (NSObject<iOSPluginLoggerDelegateInterface> * delegate in self.m_pluginLoggerDelegates) {
            [delegate onLogger:record];
        }
    }
}

- (void)config:(NSDictionary *)config {
    @autoreleasepool {
        for (NSObject<iOSPluginConfigDelegateInterface> * delegate in self.m_pluginConfigDelegates) {
            [delegate onConfig:config];
        }
    }
}

- (void)setSessionId:(iOSSessionIdParam *)sessionId {
    @autoreleasepool {
        for (NSObject<iOSPluginSessionIdDelegateInterface> * delegate in self.m_pluginSessionIdDelegates) {
            [delegate onSessionId:sessionId];
        }
    }
}

- (void)removeSessionData {
    @autoreleasepool {
        for (NSObject<iOSPluginSessionIdDelegateInterface> * delegate in self.m_pluginSessionIdDelegates) {
            [delegate onRemoveSessionData];
        }
    }
}

- (void)eventAdRevenue:(iOSAdRevenueParam *)revenue {
    @autoreleasepool {
        for (NSObject<iOSPluginAdRevenueDelegateInterface> * delegate in self.m_pluginAdRevenueDelegates) {
            [delegate onAdRevenue:revenue];
        }
    }
}

- (void)eventTransparencyConsent:(iOSTransparencyConsentParam *)consent {
    @autoreleasepool {
        for (NSObject<iOSPluginTransparencyConsentDelegateInterface> * delegate in self.m_pluginTransparencyConsentDelegates) {
            [delegate onTransparencyConsent:consent];
        }
    }
}

#pragma mark - UIApplicationDelegate Protocol

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions API_AVAILABLE(ios(3.0)) {
    [AppleLog withFormat:@"Mengine application didFinishLaunchingWithOptions"];
    
    [[iOSNetwork sharedInstance] startMonitoring];
    
    @autoreleasepool {
        if( [iOSApplication.sharedInstance didFinishLaunchingWithOptions:launchOptions] == NO) {
            [AppleLog withFormat:@"🔴 [ERROR] Mengine iOSApplication didFinishLaunchingWithOptions failed"];
            
            return NO;
        }
    }
    
    UIWindow * window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    UIViewController * viewController = [[UIViewController alloc] init];
    window.rootViewController = viewController;
    
    [window makeKeyAndVisible];
    
    [self setWindow:window];
    
    @autoreleasepool {
        for (id delegate in self.m_pluginApplicationDelegates) {
            if ([delegate application:application didFinishLaunchingWithOptions:launchOptions] == NO) {
                [AppleLog withFormat:@"🔴 [ERROR] Mengine application didFinishLaunchingWithOptions plugin %@ failed", NSStringFromClass([delegate class])];
                
                return NO;
            }
        }
    }
    
    @autoreleasepool {
        for (id delegate in self.m_pluginApplicationDelegates) {
            if ([delegate respondsToSelector:@selector(application: didPostLaunchingWithOptions:)] == NO) {
                continue;
            }
            
            if ([delegate application:application didPostLaunchingWithOptions:launchOptions] == NO) {
                [AppleLog withFormat:@"🔴 [ERROR] Mengine application didPostLaunchingWithOptions plugin %@ failed", NSStringFromClass([delegate class])];
                
                return NO;
            }
        }
    }
    
    SDL_SetMainReady();
    
    [self performSelector:@selector(postFinishLaunch) withObject:nil afterDelay:0.0];
    
    return YES;
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken API_AVAILABLE(ios(3.0)) {
    [AppleLog withFormat:@"Mengine application didRegisterForRemoteNotificationsWithDeviceToken"];

    @autoreleasepool {
        for (id delegate in self.m_pluginApplicationDelegates) {
            if ([delegate respondsToSelector:@selector(application: didRegisterForRemoteNotificationsWithDeviceToken:)] == NO) {
                continue;
            }
            
            [delegate application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
        }
    }
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult result))completionHandler API_AVAILABLE(ios(7.0)) {
    [AppleLog withFormat:@"Mengine application didReceiveRemoteNotification"];
    
    @autoreleasepool {
        for (id delegate in self.m_pluginApplicationDelegates) {
            if ([delegate respondsToSelector:@selector(application: didReceiveRemoteNotification: fetchCompletionHandler:)] == NO) {
                continue;
            }
            
            [delegate application:application didReceiveRemoteNotification:userInfo fetchCompletionHandler:completionHandler];
        }
    }
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    [AppleLog withFormat:@"Mengine application applicationDidBecomeActive"];
    
    @autoreleasepool {
        for (id delegate in self.m_pluginApplicationDelegates) {
            if ([delegate respondsToSelector:@selector(applicationDidBecomeActive:)] == NO) {
                continue;
            }
            
            [delegate applicationDidBecomeActive:application];
        }
    }
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    [AppleLog withFormat:@"Mengine application applicationWillEnterForeground"];
    
    @autoreleasepool {
        for (id delegate in self.m_pluginApplicationDelegates) {
            if ([delegate respondsToSelector:@selector(applicationWillEnterForeground:)] == NO) {
                continue;
            }
            
            [delegate applicationWillEnterForeground:application];
        }
    }
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    [AppleLog withFormat:@"Mengine application applicationDidEnterBackground"];
    
    @autoreleasepool {
        for (id delegate in self.m_pluginApplicationDelegates) {
            if ([delegate respondsToSelector:@selector(applicationDidEnterBackground:)] == NO) {
                continue;
            }
            
            [delegate applicationDidEnterBackground:application];
        }
    }
}

- (void)applicationWillResignActive:(UIApplication *)application {
    [AppleLog withFormat:@"Mengine application applicationWillResignActive"];
    
    @autoreleasepool {
        for (id delegate in self.m_pluginApplicationDelegates) {
            if ([delegate respondsToSelector:@selector(applicationWillResignActive:)] == NO) {
                continue;
            }
            
            [delegate applicationWillResignActive:application];
        }
    }
}

- (void)applicationWillTerminate:(UIApplication *)application {
    [AppleLog withFormat:@"Mengine application applicationWillTerminate"];
    
    @autoreleasepool {
        for (id delegate in self.m_pluginApplicationDelegates) {
            if ([delegate respondsToSelector:@selector(applicationWillTerminate:)] == NO) {
                continue;
            }
            
            [delegate applicationWillTerminate:application];
        }
    }
    
    [[iOSNetwork sharedInstance] stopMonitoring];
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<UIApplicationOpenURLOptionsKey, id> *)options API_AVAILABLE(ios(9.0)) {
    @autoreleasepool {
        for (id delegate in self.m_pluginApplicationDelegates) {
            if ([delegate respondsToSelector:@selector(application: openURL: options: handled:)] == NO) {
                continue;
            }
            
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
    return self.m_window;
}

- (void)setWindow:(UIWindow *)window {
    self.m_window = window;
}


- (void)postFinishLaunch {
    @autoreleasepool {
        for (id delegate in self.m_pluginApplicationDelegates) {
            if ([delegate respondsToSelector:@selector(beginLoop:)] == NO) {
                continue;
            }
            
            [delegate beginLoop];
        }
    }
    
    SDL_iPhoneSetEventPump( SDL_TRUE );
    
    Mengine::iOSApplication application;
    
    NSArray<NSString *> * arguments = [[NSProcessInfo processInfo] arguments];
    
    int32_t argc = 0;
    Mengine::Char * argv[32];
    
    for( NSString * arg : arguments )
    {
        argv[argc++] = (Mengine::Char *)[arg UTF8String];
    }
    
    if( application.bootstrap( argc, argv ) == false ) {
        [AppleLog withFormat:@"🔴 [ERROR] Mengine application bootstrap [Failed]"];
        
        [[NSOperationQueue mainQueue] cancelAllOperations];
        
        application.finalize();
        
        SDL_iPhoneSetEventPump( SDL_FALSE );
        
        [iOSDetail showOkAlertWithTitle:@"Failed..."
                                message:@"Mengine bootstraped application"
                                     ok:^{
            ::exit( EXIT_FAILURE );
        }];
        
        return;
    }

    if( application.initialize() == false ) {
        [AppleLog withFormat:@"🔴 [ERROR] Mengine application initialize [Failed]"];
        
        [[NSOperationQueue mainQueue] cancelAllOperations];
        
        application.finalize();
        
        SDL_iPhoneSetEventPump( SDL_FALSE );
        
        [iOSDetail showOkAlertWithTitle:@"Failed..."
                                message:@"Mengine initialized application"
                                     ok:^{
            ::exit( EXIT_FAILURE );
        }];
        
        return;
    }
    
    application.loop();
    
    [[NSOperationQueue mainQueue] cancelAllOperations];
    
    application.finalize();
    
    @autoreleasepool {
        for (id delegate in self.m_pluginApplicationDelegates) {
            if ([delegate respondsToSelector:@selector(endLoop:)] == NO) {
                continue;
            }
            
            [delegate endLoop];
        }
    }
    
    SDL_iPhoneSetEventPump( SDL_FALSE );
    
    [AppleLog withFormat:@"Mengine application finish"];
    
    ::exit( EXIT_SUCCESS );
}

@end
