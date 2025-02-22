#import "AppleFirebaseRemoteConfigApplicationDelegate.h"

#import "Environment/iOS/iOSNetwork.h"
#import "Environment/iOS/iOSDetail.h"

#include "AppleFirebaseRemoteConfigInterface.h"

#include "Kernel/Logger.h"

#import <FirebaseRemoteConfig/FirebaseRemoteConfig.h>

@implementation AppleFirebaseRemoteConfigApplicationDelegate

+ (AppleFirebaseRemoteConfigApplicationDelegate *) sharedInstance {
    static AppleFirebaseRemoteConfigApplicationDelegate *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleFirebaseRemoteConfigApplicationDelegate class]];
    });
    return sharedInstance;
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    FIRRemoteConfigSettings *remoteConfigSettings = [[FIRRemoteConfigSettings alloc] init];
    remoteConfigSettings.minimumFetchInterval = MENGINE_DEBUG_VALUE(0, 3600);
    
    [[FIRRemoteConfig remoteConfig] setConfigSettings:remoteConfigSettings];
    
    [[FIRRemoteConfig remoteConfig] setDefaultsFromPlistFileName:@MENGINE_FIREBASE_REMOTECONFIG_PLIST_NAME];
    
    if ([[iOSNetwork sharedInstance] isNetworkAvailable] == YES) {
        [[FIRRemoteConfig remoteConfig] fetchWithCompletionHandler:^(FIRRemoteConfigFetchStatus status, NSError *error) {
            switch (status) {
                case FIRRemoteConfigFetchStatusNoFetchYet:
                    NSLog( @"FIRRemoteConfigFetchStatusNoFetchYet: %@"
                          , [error description]
                          );
                    
                    break;
                case FIRRemoteConfigFetchStatusSuccess:
                    NSLog( @"FIRRemoteConfigFetchStatusSuccess" );
                    
                    [[FIRRemoteConfig remoteConfig] activateWithCompletion:^(BOOL changed, NSError * _Nullable error) {
                        if (error != nil) {
                            NSLog( @"FIRRemoteConfigFetchStatusSuccess activate error: %@"
                                  , [error description]
                                  );
                            
                            return;
                        }
                        
                        NSLog( @"FIRRemoteConfigFetchStatusSuccess activate changed: %d"
                              , changed
                              );
                    }];
                    break;
                case FIRRemoteConfigFetchStatusFailure:
                    NSLog( @"FIRRemoteConfigFetchStatusFailure: %@"
                          , [error description]
                          );
                    
                    break;
                case FIRRemoteConfigFetchStatusThrottled:
                    NSLog( @"FIRRemoteConfigFetchStatusThrottled: %@"
                          , [error description]
                          );
                    
                    break;
                default:
                    break;
            }
        }];
    }
    
    return YES;
}

@end
