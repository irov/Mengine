#import "AppleFirebaseApplicationDelegate.h"

#import "Environment/iOS/iOSDetail.h"

#import <Firebase/Firebase.h>

@implementation AppleFirebaseApplicationDelegate

+ (instancetype) sharedInstance {
    static AppleFirebaseApplicationDelegate *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleFirebaseApplicationDelegate class]];
    });
    return sharedInstance;
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    @try {
#if defined(MENGINE_DEBUG)
        [[FIRConfiguration sharedInstance] setLoggerLevel:FIRLoggerLevelInfo];
#endif
        
        [FIRApp configure];
    } @catch (NSException * ex) {
        NSLog( @"firebase configure caught exception [%@]: %@"
            , ex.name
            , ex.reason
        );
        
        return NO;
    }
    
    return YES;
}

@end
