#import "iOSUIApplicationDelegate.h"
#import "iOSSceneDelegate.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/PluginServiceInterface.h"

#import "iOSApplicationDelegates.h"
#import "iOSViewController.h"

#import "Environment/Apple/AppleUserDefaults.h"
#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleLog.h"
#import "Environment/Apple/AppleString.h"
#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSNetwork.h"
#import "Environment/iOS/iOSDetail.h"
#include "Environment/iOS/iOSPlatformServiceExtensionInterface.h"

#include "iOSApplication.h"

#include "Kernel/NotificationHelper.h"

#ifndef MENGINE_IOS_LAUNCH_ARGUMENTS_CAPACITY
#define MENGINE_IOS_LAUNCH_ARGUMENTS_CAPACITY 32
#endif

@interface iOSUIApplicationDelegate ()

- (void)startEngineLoop;
- (void)stopEngineLoop;
- (void)engineFrame:(CADisplayLink *)displayLink;
- (void)finishApplication;

@end

@implementation iOSUIApplicationDelegate

- (instancetype)init {
    if (self = [super init]) {
        self.m_application = nullptr;
        self.m_displayLink = nil;
        self.m_prevTimestamp = 0.0;
        self.m_pluginDelegates = [NSMutableArray<id> array];
        self.m_plugins = [NSMutableArray<iOSPluginInterface> array];
        self.m_pluginLoggerDelegates = [NSMutableArray<iOSPluginLoggerDelegateInterface> array];
        self.m_pluginConfigDelegates = [NSMutableArray<iOSPluginConfigDelegateInterface> array];
        self.m_pluginAnalyticDelegates = [NSMutableArray<iOSPluginAnalyticDelegateInterface> array];
        self.m_pluginUserIdDelegates = [NSMutableArray<iOSPluginUserIdDelegateInterface> array];
        self.m_pluginAdRevenueDelegates = [NSMutableArray<iOSPluginAdRevenueDelegateInterface> array];
        self.m_pluginAppTrackingTransparencyDelegates = [NSMutableArray<iOSPluginAppTrackingTransparencyDelegateInterface> array];
        self.m_pluginTransparencyConsentDelegates = [NSMutableArray<iOSPluginTransparencyConsentDelegateInterface> array];
        
        self.m_didBecomeActiveOperations = [NSMutableArray<iOSDidBecomeActiveOperationBlock> array];
        self.m_isProcessingDidBecomeActiveOperation = NO;
        self.m_isApplicationForeground = NO;
        self.m_isApplicationActive = NO;
        self.m_isApplicationTerminating = NO;
        
        NSArray * proxysClassed = [iOSApplicationDelegates getApplicationDelegates];
        
        for (id className in proxysClassed) {
            id c = NSClassFromString(className);
            
            if (c == nil) {
                continue;
            }
            
            id delegate = [[c alloc] init];
            
            [self.m_pluginDelegates addObject:delegate];
            
            if ([delegate conformsToProtocol:@protocol(iOSPluginInterface)] == YES) {
                [self.m_plugins addObject:delegate];
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

- (NSArray<iOSPluginInterface> *)getPlugins {
    return self.m_plugins;
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

    for( id arg = firstArg; arg != nil; arg = va_arg(args, id) ) {
        [send_args addObject:arg];
    }

    va_end(args);
    
    [self notify:event arrayArgs:send_args];
}

- (void)notify:(AppleEvent *)event arrayArgs:(NSArray<id> *)args {
    @autoreleasepool {
        for (NSObject<iOSPluginInterface> * delegate in self.m_plugins) {
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
    
    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin application:application didFinishLaunchingWithOptions:launchOptions] == NO) {
                [AppleLog withFormat:@"🔴 [ERROR] Mengine application didFinishLaunchingWithOptions plugin %@ failed", NSStringFromClass([plugin class])];
                
                return NO;
            }
        }
    }
    
    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(application: didPostLaunchingWithOptions:)] == NO) {
                continue;
            }
            
            if ([plugin application:application didPostLaunchingWithOptions:launchOptions] == NO) {
                [AppleLog withFormat:@"🔴 [ERROR] Mengine application didPostLaunchingWithOptions plugin %@ failed", NSStringFromClass([plugin class])];
                
                return NO;
            }
        }
    }
    
    [self performSelector:@selector(postFinishLaunch) withObject:nil afterDelay:0.0];
    
    return YES;
}

#pragma mark - UISceneSession lifecycle

- (UISceneConfiguration *)application:(UIApplication *)application configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession options:(UISceneConnectionOptions *)options API_AVAILABLE(ios(13.0)) {
    UISceneConfiguration * configuration = [[UISceneConfiguration alloc] initWithName:@"Default Configuration" sessionRole:connectingSceneSession.role];
    configuration.delegateClass = [iOSSceneDelegate class];
    
    return configuration;
}

- (void)application:(UIApplication *)application didDiscardSceneSessions:(NSSet<UISceneSession *> *)sceneSessions API_AVAILABLE(ios(13.0)) {
    // Called when the user discards a scene session
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken API_AVAILABLE(ios(3.0)) {
    [AppleLog withFormat:@"Mengine application didRegisterForRemoteNotificationsWithDeviceToken"];

    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(application: didRegisterForRemoteNotificationsWithDeviceToken:)] == NO) {
                continue;
            }
            
            [plugin application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
        }
    }
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult result))completionHandler API_AVAILABLE(ios(7.0)) {
    [AppleLog withFormat:@"Mengine application didReceiveRemoteNotification"];
    
    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(application: didReceiveRemoteNotification: fetchCompletionHandler:)] == NO) {
                continue;
            }
            
            [plugin application:application didReceiveRemoteNotification:userInfo fetchCompletionHandler:completionHandler];
        }
    }
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    [self handleApplicationDidBecomeActive:application];
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    [self handleApplicationWillEnterForeground:application];
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    [self handleApplicationDidEnterBackground:application];
}

- (void)applicationWillResignActive:(UIApplication *)application {
    [self handleApplicationWillResignActive:application];
}

- (void)applicationWillTerminate:(UIApplication *)application {
    [self handleApplicationWillTerminate:application];

    [[iOSNetwork sharedInstance] stopMonitoring];
}

- (void)handleApplicationDidBecomeActive:(UIApplication *)application {
    if (self.m_isApplicationActive == YES) {
        return;
    }

    self.m_isApplicationActive = YES;

    [AppleLog withFormat:@"Mengine application applicationDidBecomeActive"];

    if (self.m_isApplicationTerminating == NO) {
        [self processNextOperation];
    }

    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(applicationDidBecomeActive:)] == NO) {
                continue;
            }

            [plugin applicationDidBecomeActive:application];
        }
    }

    if( SERVICE_PROVIDER_EXIST() == true && SERVICE_IS_INITIALIZE( Mengine::PlatformServiceInterface ) == true ) {
        Mengine::iOSPlatformServiceExtensionInterface * extension = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        if( extension != nullptr ) {
            extension->handleApplicationDidBecomeActive();
        }
    }
}

- (void)handleApplicationWillEnterForeground:(UIApplication *)application {
    if (self.m_isApplicationForeground == YES) {
        return;
    }

    self.m_isApplicationForeground = YES;

    [AppleLog withFormat:@"Mengine application applicationWillEnterForeground"];

    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(applicationWillEnterForeground:)] == NO) {
                continue;
            }

            [plugin applicationWillEnterForeground:application];
        }
    }

    if( SERVICE_PROVIDER_EXIST() == true && SERVICE_IS_INITIALIZE( Mengine::PlatformServiceInterface ) == true ) {
        Mengine::iOSPlatformServiceExtensionInterface * extension = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        if( extension != nullptr ) {
            extension->handleApplicationWillEnterForeground();
        }
    }
}

- (void)handleApplicationDidEnterBackground:(UIApplication *)application {
    if (self.m_isApplicationForeground == NO) {
        return;
    }

    self.m_isApplicationForeground = NO;

    [AppleLog withFormat:@"Mengine application applicationDidEnterBackground"];

    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(applicationDidEnterBackground:)] == NO) {
                continue;
            }

            [plugin applicationDidEnterBackground:application];
        }
    }

    if( SERVICE_PROVIDER_EXIST() == true && SERVICE_IS_INITIALIZE( Mengine::PlatformServiceInterface ) == true ) {
        Mengine::iOSPlatformServiceExtensionInterface * extension = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        if( extension != nullptr ) {
            extension->handleApplicationDidEnterBackground();
        }
    }
}

- (void)handleApplicationWillResignActive:(UIApplication *)application {
    if (self.m_isApplicationActive == NO) {
        return;
    }

    self.m_isApplicationActive = NO;

    [AppleLog withFormat:@"Mengine application applicationWillResignActive"];

    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(applicationWillResignActive:)] == NO) {
                continue;
            }

            [plugin applicationWillResignActive:application];
        }
    }

    if( SERVICE_PROVIDER_EXIST() == true && SERVICE_IS_INITIALIZE( Mengine::PlatformServiceInterface ) == true ) {
        Mengine::iOSPlatformServiceExtensionInterface * extension = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        if( extension != nullptr ) {
            extension->handleApplicationWillResignActive();
        }
    }
}

- (void)handleApplicationWillTerminate:(UIApplication *)application {
    if (self.m_isApplicationTerminating == YES) {
        return;
    }

    self.m_isApplicationTerminating = YES;
    self.m_isApplicationActive = NO;
    self.m_isApplicationForeground = NO;

    [AppleLog withFormat:@"Mengine application applicationWillTerminate"];

    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(applicationWillTerminate:)] == NO) {
                continue;
            }

            [plugin applicationWillTerminate:application];
        }
    }

    if( SERVICE_PROVIDER_EXIST() == true && SERVICE_IS_INITIALIZE( Mengine::PlatformServiceInterface ) == true ) {
        Mengine::iOSPlatformServiceExtensionInterface * extension = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        if( extension != nullptr ) {
            extension->handleApplicationWillTerminate();
        }
    }
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<UIApplicationOpenURLOptionsKey, id> *)options API_AVAILABLE(ios(9.0)) {
    BOOL anySuccess = NO;

    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(application: openURL: options:)] == NO) {
                continue;
            }

            BOOL result = [plugin application:application openURL:url options:options];

            if (result == YES) {
                anySuccess = YES;
            }
        }
    }

    return anySuccess;
}

- (BOOL)application:(UIApplication *)application continueUserActivity:(NSUserActivity *)userActivity restorationHandler:(void (^)(NSArray<id<UIUserActivityRestoring>> * _Nullable restorableObjects))restorationHandler {
    BOOL anySuccess = NO;

    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(application:continueUserActivity:restorationHandler:)] == NO) {
                continue;
            }

            BOOL result = [plugin application:application continueUserActivity:userActivity restorationHandler:restorationHandler];

            if (result == YES) {
                anySuccess = YES;
            }
        }
    }

    return anySuccess;
}

- (UIWindow *)window {
    return self.m_window;
}

- (void)setWindow:(UIWindow *)window {
    self.m_window = window;
}

- (void)startEngineLoop {
    if (self.m_displayLink != nil) {
        return;
    }

    self.m_prevTimestamp = 0.0;

    CADisplayLink * displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(engineFrame:)];
    [displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];

    self.m_displayLink = displayLink;
}

- (void)stopEngineLoop {
    if (self.m_displayLink == nil) {
        return;
    }

    [self.m_displayLink invalidate];
    self.m_displayLink = nil;
    self.m_prevTimestamp = 0.0;
}

- (void)engineFrame:(CADisplayLink *)displayLink {
    if (self.m_application == nullptr) {
        [self stopEngineLoop];

        return;
    }

    NOTIFICATION_NOTIFY( Mengine::NOTIFICATOR_PLATFORM_UPDATE );

    if( PLATFORM_SERVICE()
        ->updatePlatform() == false )
    {
        [self finishApplication];

        return;
    }

    CFTimeInterval frameTime = (self.m_prevTimestamp > 0.0)
        ? ((displayLink.timestamp - self.m_prevTimestamp) * 1000.0)
        : (displayLink.duration * 1000.0);

    self.m_prevTimestamp = displayLink.timestamp;

    float frameTimeF = (float)frameTime;

    PLATFORM_SERVICE()
        ->tickPlatform( frameTimeF );

    PLATFORM_SERVICE()
        ->renderPlatform();
}

- (void)finishApplication {
    if (self.m_application == nullptr) {
        return;
    }

    [self handleApplicationWillTerminate:[UIApplication sharedApplication]];

    [self stopEngineLoop];

    Mengine::iOSApplication * application = static_cast<Mengine::iOSApplication *>(self.m_application);

    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(onLoopEnd)] == NO) {
                continue;
            }

            [plugin onLoopEnd];
        }
    }

    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(onStopBegin)] == NO) {
                continue;
            }

            [plugin onStopBegin];
        }
    }

    application->stop();

    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(onStopEnd)] == NO) {
                continue;
            }

            [plugin onStopEnd];
        }
    }

    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(onFinalize)] == NO) {
                continue;
            }

            [plugin onFinalize];
        }
    }

    application->finalize();
    delete application;
    self.m_application = nullptr;

    [AppleLog withFormat:@"Mengine application finish"];

    [AppleDetail cancelAllQueueOperations];

    ::exit( EXIT_SUCCESS );
}


- (void)postFinishLaunch {
    Mengine::iOSApplication * application = new Mengine::iOSApplication();
    self.m_application = application;
    
    NSArray<NSString *> * arguments = [[NSProcessInfo processInfo] arguments];
    
    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(onBootstrapBegin:)] == NO) {
                continue;
            }
            
            [plugin onBootstrapBegin:arguments];
        }
    }
    
    int32_t argc = 0;
    Mengine::Char * argv[MENGINE_IOS_LAUNCH_ARGUMENTS_CAPACITY]= {nullptr};
    
    for( NSString * arg : arguments )
    {
        if( (size_t)argc >= MENGINE_IOS_LAUNCH_ARGUMENTS_CAPACITY ) {
            [AppleLog withFormat:@"🔴 [ERROR] Mengine application too many launch arguments (%lu), truncated to %lu"
                , (unsigned long)[arguments count]
                , (unsigned long)MENGINE_IOS_LAUNCH_ARGUMENTS_CAPACITY
            ];

            break;
        }

        argv[argc++] = (Mengine::Char *)[arg UTF8String];
    }    
    
    if( application->bootstrap( argc, argv ) == false ) {
        [AppleLog withFormat:@"🔴 [ERROR] Mengine application bootstrap [Failed]"];
        
        application->finalize();
        delete application;
        self.m_application = nullptr;
        
        [iOSDetail showOkAlertWithTitle:@"Failed..."
                                message:@"Mengine bootstraped application"
                                     ok:^{
            [AppleDetail cancelAllQueueOperations];
            
            ::exit( EXIT_FAILURE );
        }];
        
        return;
    }

    if( SERVICE_IS_INITIALIZE( Mengine::PluginServiceInterface ) == true ) {
        NSArray<NSString *> * delegates = [iOSApplicationDelegates getApplicationDelegates];

        for( NSString * pluginNameString in delegates ) {
            Mengine::ConstString pluginName = [AppleString NSStringToConstString:pluginNameString];

            PLUGIN_SERVICE()
                ->setAvailablePlugin( pluginName, true );
        }
    }
    
    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(onBootstrapEnd)] == NO) {
                continue;
            }
            
            [plugin onBootstrapEnd];
        }
    }
        
    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(onRunBegin)] == NO) {
                continue;
            }
            
            [plugin onRunBegin];
        }
    }
    
    if( application->run() == false ) {
        [AppleLog withFormat:@"🔴 [ERROR] Mengine application run [Failed]"];
        
        application->finalize();
        delete application;
        self.m_application = nullptr;
        
        [iOSDetail showOkAlertWithTitle:@"Failed..."
                                message:@"Mengine run application"
                                     ok:^{
            [AppleDetail cancelAllQueueOperations];
            
            ::exit( EXIT_FAILURE );
        }];
        
        return;
    }
    
    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(onRunEnd)] == NO) {
                continue;
            }
            
            [plugin onRunEnd];
        }
    }
    
    @autoreleasepool {
        for (id plugin in self.m_plugins) {
            if ([plugin respondsToSelector:@selector(onLoopBegin)] == NO) {
                continue;
            }
            
            [plugin onLoopBegin];
        }
    }

    [self startEngineLoop];
}

@end
