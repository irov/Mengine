#import "AppleSKAdNetworkApplicationDelegate.h"

#import <StoreKit/SKAdNetwork.h>

@implementation AppleSKAdNetworkApplicationDelegate

#pragma mark - UIApplicationDelegate Protocol

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    if (@available(iOS 15.4, *)) {
        //
    } else if (@available(iOS 11.3, *)) {
        [SKAdNetwork registerAppForAdNetworkAttribution];
    }
    
    return YES;
}

@end
