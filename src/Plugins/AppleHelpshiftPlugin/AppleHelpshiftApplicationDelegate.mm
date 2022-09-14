#import "AppleHelpshiftApplicationDelegate.h"

#import <HelpshiftX/Helpshift.h>

@implementation AppleHelpshiftApplicationDelegate

#pragma mark -

+ (void)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    NSDictionary *config = @{
        @"enableLogging":@YES,
        
//        @"enableInAppNotification":@YES,
        
//        @"inAppNotificationAppearance": @{
//            @"bannerBackgroundColor": @"000000",
//            @"textColor": @"FFFFFF"
//        }
        
//        @"presentFullScreenOniPad":@YES,
    };
    
    NSBundle* mainBundle = [NSBundle mainBundle];
    NSString *platform_id = [mainBundle objectForInfoDictionaryKey:@"AppleHelpshiftPlugin_Platform_Id"];
    NSString *domain = [mainBundle objectForInfoDictionaryKey:@"AppleHelpshiftPlugin_Domain"];
    
    [Helpshift installWithPlatformId:platform_id
                              domain:domain
                              config:config];
    
}

#pragma mark - UISceneSession lifecycle

+ (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
}

+ (void)application:(UIApplication *)application didReceiveRemoteNotification:(nonnull NSDictionary *)userInfo {
}

+ (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void(^)(UIBackgroundFetchResult)) completionHandler {
}

+ (void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification {
}

+ (void)applicationWillResignActive:(UIApplication *)application {
}

+ (void)applicationDidEnterBackground:(UIApplication *)application {
}

+ (void)applicationWillEnterForeground:(UIApplication *)application {
}

+ (void)applicationDidBecomeActive:(UIApplication *)application {
}

+ (void)applicationWillTerminate:(UIApplication *)application {

}

+ (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url {
    return false;
}

+ (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation {
    return  false;
}

+ (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<NSString *,id> *)options {
    return  false;
}

@end
