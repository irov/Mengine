#import "AppleAdvertisementApplicationDelegate.h"

#import "Environment/iOS/iOSDetail.h"

@implementation AppleAdvertisementApplicationDelegate

+ (AppleAdvertisementApplicationDelegate *) sharedInstance {
    static AppleAdvertisementApplicationDelegate *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleAdvertisementApplicationDelegate class]];
    });
    return sharedInstance;
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    return YES;
}

@end
