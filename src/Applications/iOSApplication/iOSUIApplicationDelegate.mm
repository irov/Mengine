#import "iOSUIApplicationDelegate.h"

#include "Interface/PlatformServiceInterface.h"

#include "Environment/SDL3/SDL3Includer.h"
#include "Environment/SDL3/SDL3PlatformServiceExtensionInterface.h"

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
        self.m_pluginDelegates = [NSMutableArray<id> array];
        self.m_pluginApplicationDelegates = [NSMutableArray<iOSPluginApplicationDelegateInterface> array];
        self.m_pluginLoggerDelegates = [NSMutableArray<iOSPluginLoggerDelegateInterface> array];
        self.m_pluginConfigDelegates = [NSMutableArray<iOSPluginConfigDelegateInterface> array];
        self.m_pluginAnalyticDelegates = [NSMutableArray<iOSPluginAnalyticDelegateInterface> array];
        self.m_pluginUserIdDelegates = [NSMutableArray<iOSPluginUserIdDelegateInterface> array];
        self.m_pluginAdRevenueDelegates = [NSMutableArray<iOSPluginAdRevenueDelegateInterface> array];
        self.m_pluginAppTrackingTransparencyDelegates = [NSMutableArray<iOSPluginAppTrackingTransparencyDelegateInterface> array];
        self.m_pluginTransparencyConsentDelegates = [NSMutableArray<iOSPluginTransparencyConsentDelegateInterface> array];
        
        self.m_didBecomeActiveOperations = [NSMutableArray<iOSDidBecomeActiveOperationBlock> array];
        self.m_isProcessingDidBecomeActiveOperation = NO;
        
        NSArray * proxysClassed = [iOSApplicationDelegates getApplicationDelegates];
        
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
            
            if ([delegate conformsToProtocol:@protocol(iOSPluginAnalyticDelegateInterface)] == YES) {
                [self.m_pluginAnalyticDelegates addObject:delegate];
            }
            
            if ([delegate conformsToProtocol:@protocol(iOSPluginUserIdDelegateInterface)] == YES) {
                [self.m_pluginUserIdDelegates addObject:delegate];
            }
            
            if ([delegate conformsToProtocol:@protocol(iOSPluginAdRevenueDelegateInterface)] == YES) {
                [self.m_pluginAdRevenueDelegates addObject:delegate];
            }
            
            if ([delegate conformsToProtocol:@protocol(iOSPluginAppTrackingTransparencyDelegateInterface)] == YES) {
                [self.m_pluginAppTrackingTransparencyDelegates addObject:delegate];
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

- (NSArray<iOSPluginAnalyticDelegateInterface> *)getPluginAnalyticDelegates {
    return self.m_pluginAnalyticDelegates;
}

- (NSArray<iOSPluginUserIdDelegateInterface> *)getPluginUserIdDelegates {
    return self.m_pluginUserIdDelegates;
}

- (NSArray<iOSPluginAdRevenueDelegateInterface> *)getPluginAdRevenueDelegates {
    return self.m_pluginAdRevenueDelegates;
}

- (NSArray<iOSPluginAppTrackingTransparencyDelegateInterface> *)getPluginAppTrackingTransparencyDelegates {
    return self.m_pluginAppTrackingTransparencyDelegates;
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

- (void)eventLog:(AppleLogRecordParam *)record {
    @autoreleasepool {
        for (NSObject<iOSPluginLoggerDelegateInterface> * delegate in self.m_pluginLoggerDelegates) {
            [delegate onLogger:record];
        }
    }
}

- (void)eventConfig:(NSDictionary *)config ids:(NSDictionary *)ids {
    @autoreleasepool {
        for (NSObject<iOSPluginConfigDelegateInterface> * delegate in self.m_pluginConfigDelegates) {
            [delegate onConfig:config ids:ids];
        }
    }
}

- (void)eventAnalytic:(NSString * _Nonnull)event category:(iOSAnalyticsEventCategory)category params:(NSDictionary * _Nonnull)params {
    @autoreleasepool {
        for (NSObject<iOSPluginAnalyticDelegateInterface> * delegate in self.m_pluginAnalyticDelegates) {
            [delegate onAnalyticEvent:event category:category params:params];
        }
    }
}

- (void)eventAnalyticScreen:(NSString *)name type:(NSString *)type {
    @autoreleasepool {
        for (NSObject<iOSPluginAnalyticDelegateInterface> * delegate in self.m_pluginAnalyticDelegates) {
            [delegate onAnalyticScreen:name type:type];
        }
    }
}

- (void)eventAnalyticFlush {
    @autoreleasepool {
        for (NSObject<iOSPluginAnalyticDelegateInterface> * delegate in self.m_pluginAnalyticDelegates) {
            [delegate onAnalyticFlush];
        }
    }
}

- (void)eventSetUserId:(iOSUserParam *)userId {
    @autoreleasepool {
        for (NSObject<iOSPluginUserIdDelegateInterface> * delegate in self.m_pluginUserIdDelegates) {
            [delegate onUserId:userId];
        }
    }
}

- (void)eventRemoveUserData {
    @autoreleasepool {
        for (NSObject<iOSPluginUserIdDelegateInterface> * delegate in self.m_pluginUserIdDelegates) {
            [delegate onRemoveUserData];
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

- (void)eventAppTrackingTransparency:(iOSAppTrackingTransparencyParam * _Nonnull)tracking {
    @autoreleasepool {
        for (NSObject<iOSPluginAppTrackingTransparencyDelegateInterface> * delegate in self.m_pluginAppTrackingTransparencyDelegates) {
            [delegate onAppTrackingTransparency:tracking];
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

- (void)addDidBecomeActiveOperationWithCompletion:(iOSDidBecomeActiveOperationBlock)block {
    @synchronized(self.m_didBecomeActiveOperations) {
        [self.m_didBecomeActiveOperations addObject:block];
    }
        
    [self processNextOperation];
}

- (void)processNextOperation {
    [AppleDetail addMainQueueOperation:^{
        UIApplicationState appState = [[UIApplication sharedApplication] applicationState];
        
        if (appState != UIApplicationStateActive) {
            return;
        }

        iOSDidBecomeActiveOperationBlock operation = nil;
        
        @synchronized(self.m_didBecomeActiveOperations) {
            if (self.m_isProcessingDidBecomeActiveOperation == YES) {
                return;
            }
            
            if ([self.m_didBecomeActiveOperations count] == 0) {
                return;
            }
            
            operation = [self.m_didBecomeActiveOperations firstObject];
            [self.m_didBecomeActiveOperations removeObjectAtIndex:0];

            self.m_isProcessingDidBecomeActiveOperation = YES;
        }
        
        if (operation != nil) {
            [self processOperation:operation];
        }
    }];
}

- (void)processOperation:(iOSDidBecomeActiveOperationBlock)block {
    [AppleDetail addMainQueueOperation:^{
        block(^{
            @synchronized(self.m_didBecomeActiveOperations) {
                self.m_isProcessingDidBecomeActiveOperation = NO;
            }
            
            [self processNextOperation];
        });
    }];
}

#pragma mark - UIApplicationDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions API_AVAILABLE(ios(3.0)) {
    [AppleLog withFormat:@"Mengine application '%@' didFinishLaunchingWithOptions"
        , [[NSBundle mainBundle] bundleIdentifier]
    ];
    
    [[iOSNetwork sharedInstance] startMonitoring];
    
    @autoreleasepool {
        if( [iOSApplication.sharedInstance didFinishLaunchingWithOptions:launchOptions] == NO) {
            [AppleLog withFormat:@"🔴 [ERROR] Mengine application didFinishLaunchingWithOptions failed"];
            
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
    
    [self processNextOperation];
    
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
    
    SDL_SetiOSEventPump( true );
    
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
        
        [AppleDetail cancelAllQueueOperations];
        
        application.finalize();
        
        SDL_SetiOSEventPump( false );
        
        [iOSDetail showOkAlertWithTitle:@"Failed..."
                                message:@"Mengine bootstraped application"
                                     ok:^{
            ::exit( EXIT_FAILURE );
        }];
        
        return;
    }

    if( application.initialize() == false ) {
        [AppleLog withFormat:@"🔴 [ERROR] Mengine application initialize [Failed]"];
        
        [AppleDetail cancelAllQueueOperations];
        
        application.finalize();
        
        SDL_SetiOSEventPump( false );
        
        [iOSDetail showOkAlertWithTitle:@"Failed..."
                                message:@"Mengine initialized application"
                                     ok:^{
            ::exit( EXIT_FAILURE );
        }];
        
        return;
    }
    
    application.loop();
    
    [AppleDetail cancelAllQueueOperations];
    
    application.finalize();
    
    @autoreleasepool {
        for (id delegate in self.m_pluginApplicationDelegates) {
            if ([delegate respondsToSelector:@selector(endLoop:)] == NO) {
                continue;
            }
            
            [delegate endLoop];
        }
    }
    
    SDL_SetiOSEventPump( false );
    
    [AppleLog withFormat:@"Mengine application finish"];
    
    ::exit( EXIT_SUCCESS );
}

@end
