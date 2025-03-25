#import "AppleDatadogApplicationDelegate.h"

#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"

#define PLUGIN_BUNDLE_NAME "MengineAppleDatadogPlugin"

@implementation AppleDatadogApplicationDelegate

+ (instancetype) sharedInstance {
    static AppleDatadogApplicationDelegate *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleDatadogApplicationDelegate class]];
    });
    return sharedInstance;
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    return YES;
}

@end
