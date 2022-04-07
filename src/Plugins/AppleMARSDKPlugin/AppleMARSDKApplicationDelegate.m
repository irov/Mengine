#import "AppleMARSDKApplicationDelegate.h"

#import "MARSDKCore/MARSDKCore.h"

bool _hasInited = false;

@interface ShortMARSDKDelegate:NSObject<MARSDKDelegate>
@end

@implementation ShortMARSDKDelegate

- (void) OnPlastformInit: (NSDictionary *)params {
    NSLog(@"Success");
    _hasInited = true;
}

- (UIView *)GetView {
    UIWindow * appWindow = [UIApplication sharedApplication].delegate.window;
    return  [appWindow.subviews objectAtIndex:0];
}

- (UIViewController *)GetViewController {
    UIWindow * appWindow = [UIApplication sharedApplication].delegate.window;
    return  appWindow.rootViewController;
}

@end

@implementation AppleMARSDKApplicationDelegate

#pragma mark -

+ (BOOL) hasInited {
    return _hasInited;
}

+ (void)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [MARSDK sharedInstance].delegate = [[ShortMARSDKDelegate alloc] init];
    
    NSDictionary *sdkconfig = [[NSBundle mainBundle] objectForInfoDictionaryKey: @"MARSDK"];
    [[MARSDK sharedInstance] initWithParams:sdkconfig];
    [[MARSDK sharedInstance] application:application didFinishLaunchingWithOptions:launchOptions];
}

+ (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken
{
    [[MARSDK sharedInstance] application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
}

+ (void)application:(UIApplication *)application didReceiveRemoteNotification:(nonnull NSDictionary *)userInfo{
    [[MARSDK sharedInstance] application:application didReceiveRemoteNotification: userInfo];
}

+ (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void(^)(UIBackgroundFetchResult)) completionHandler{
    [[MARSDK sharedInstance] application:application didReceiveRemoteNotification: userInfo];
}

+ (void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification
{
    [[MARSDK sharedInstance] application:application didReceiveLocalNotification:notification];
}

+ (void)applicationWillResignActive:(UIApplication *)application {
    /* Sent when the application is about to move from active to inactive state. T
     his can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins
     the transition to the background state.
     */
    
    // Use this method to pause ongoing tasks, disable timers, and throttle down O penGL ES frame rates. Games should use this method to pause the game.
    [[MARSDK sharedInstance] applicationWillResignActive:application];
}

+ (void)applicationDidEnterBackground:(UIApplication *)application {
    
    // Use this method to release shared resources, save user data, invalidate tim ers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called in stead of applicationWillTerminate: when the user quits.
    [[MARSDK sharedInstance] applicationDidEnterBackground:application];
}

+ (void)applicationWillEnterForeground:(UIApplication *)application {
    NSLog(@"application will enter foreground...");
    [[MARSDK sharedInstance] applicationWillEnterForeground:application];
}

+ (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the applicati on was inactive. If the application was previously in the background, optionally refresh the user interface.
    [[MARSDK sharedInstance] applicationWillEnterForeground:application];
}

+ (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriat e. See also applicationDidEnterBackground:.
    [[MARSDK sharedInstance] applicationWillTerminate:application];
}

+ (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url
{
    return [[MARSDK sharedInstance] application:application handleOpenURL:url];
}

+ (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
    return [[MARSDK sharedInstance] application:application openURL:url sourceApplication:sourceApplication annotation:annotation];
}

+ (BOOL)application:(UIApplication *)app openURL:(NSURL *)url options:(NSDictionary<NSString *,id> *)options
{
    return [[MARSDK sharedInstance] application:app openURL:url options:options];
}

@end
