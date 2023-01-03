#import "AppleFirebaseApplicationDelegate.h"

#import <Firebase/Firebase.h>

@implementation AppleFirebaseApplicationDelegate

#pragma mark - UIKitProxyApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [FIRApp configure];
    
    return YES;
}

@end
