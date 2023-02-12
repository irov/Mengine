#import "AppleSKAdNetworkApplicationDelegate.h"

@implementation AppleSKAdNetworkApplicationDelegate

#pragma mark - UIApplicationDelegate Protocol

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    if (@available(iOS 11.3, *)) {
        [SKAdNetwork registerAppForAdNetworkAttribution];
    }
    
    return YES;
}

@end
