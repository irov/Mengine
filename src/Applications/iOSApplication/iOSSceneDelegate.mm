#import "iOSSceneDelegate.h"
#import "iOSUIApplicationDelegate.h"
#import "iOSViewController.h"

#import "Environment/Apple/AppleLog.h"

@implementation iOSSceneDelegate

- (void)scene:(UIScene *)scene willConnectToSession:(UISceneSession *)session options:(UISceneConnectionOptions *)connectionOptions API_AVAILABLE(ios(13.0)) {
    [AppleLog withFormat:@"Mengine scene willConnectToSession"];

    UIWindowScene * windowScene = (UIWindowScene *)scene;

    UIWindow * window = [[UIWindow alloc] initWithWindowScene:windowScene];
    iOSViewController * viewController = [[iOSViewController alloc] init];
    window.rootViewController = viewController;

    [window makeKeyAndVisible];

    self.window = window;

    iOSUIApplicationDelegate * appDelegate = (iOSUIApplicationDelegate *)[UIApplication sharedApplication].delegate;
    [appDelegate setWindow:window];
}

- (void)sceneDidDisconnect:(UIScene *)scene API_AVAILABLE(ios(13.0)) {
    [AppleLog withFormat:@"Mengine scene sceneDidDisconnect"];
}

- (void)sceneDidBecomeActive:(UIScene *)scene API_AVAILABLE(ios(13.0)) {
    [AppleLog withFormat:@"Mengine scene sceneDidBecomeActive"];
}

- (void)sceneWillResignActive:(UIScene *)scene API_AVAILABLE(ios(13.0)) {
    [AppleLog withFormat:@"Mengine scene sceneWillResignActive"];
}

- (void)sceneWillEnterForeground:(UIScene *)scene API_AVAILABLE(ios(13.0)) {
    [AppleLog withFormat:@"Mengine scene sceneWillEnterForeground"];
}

- (void)sceneDidEnterBackground:(UIScene *)scene API_AVAILABLE(ios(13.0)) {
    [AppleLog withFormat:@"Mengine scene sceneDidEnterBackground"];
}

@end
