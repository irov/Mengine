#import "AppleFirebasePlugin.h"

#import "Environment/iOS/iOSDetail.h"

#import <FirebaseCore/FirebaseCore.h>

@implementation AppleFirebasePlugin

+ (instancetype) sharedInstance {
    static AppleFirebasePlugin *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleFirebasePlugin class]];
    });
    return sharedInstance;
}

#pragma mark - iOSPluginInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    @try {
#if defined(MENGINE_DEBUG)
        [[FIRConfiguration sharedInstance] setLoggerLevel:FIRLoggerLevelWarning];
#else
        [[FIRConfiguration sharedInstance] setLoggerLevel:FIRLoggerLevelError];
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
