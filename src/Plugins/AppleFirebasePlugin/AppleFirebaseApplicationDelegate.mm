#import "AppleFirebaseApplicationDelegate.h"

#import <Firebase/Firebase.h>

@implementation AppleFirebaseApplicationDelegate

#pragma mark - UIKitProxyApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    @try {
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
