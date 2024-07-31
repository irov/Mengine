#import "AppleFirebaseApplicationDelegate.h"

#import <Firebase/Firebase.h>

@implementation AppleFirebaseApplicationDelegate

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
