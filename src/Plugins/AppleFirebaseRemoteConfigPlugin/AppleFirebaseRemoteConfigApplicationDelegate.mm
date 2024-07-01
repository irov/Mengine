#import "AppleFirebaseRemoteConfigApplicationDelegate.h"

#include "AppleFirebaseRemoteConfigInterface.h"

#include "Kernel/Logger.h"

#import <FirebaseRemoteConfig/FirebaseRemoteConfig.h>

@implementation AppleFirebaseRemoteConfigApplicationDelegate

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    FIRRemoteConfigSettings *remoteConfigSettings = [[FIRRemoteConfigSettings alloc] init];
    remoteConfigSettings.minimumFetchInterval = 3600;
    [[FIRRemoteConfig remoteConfig] setConfigSettings:remoteConfigSettings];
    
    [[FIRRemoteConfig remoteConfig] setDefaultsFromPlistFileName:@"RemoteConfigDefaults"];
    
    [[FIRRemoteConfig remoteConfig] fetchWithCompletionHandler:^(FIRRemoteConfigFetchStatus status, NSError *error) {
        switch (status) {
            case FIRRemoteConfigFetchStatusNoFetchYet:
                NSLog( @"FIRRemoteConfigFetchStatusNoFetchYet: %@"
                    , [error localizedDescription]
                );
                
                break;
            case FIRRemoteConfigFetchStatusSuccess:
                NSLog( @"FIRRemoteConfigFetchStatusSuccess" );
                
                [[FIRRemoteConfig remoteConfig] activateWithCompletion:^(BOOL changed, NSError * _Nullable error) {
                    if (error != nil)
                    {
                        NSLog( @"FIRRemoteConfigFetchStatusSuccess activate error: %@"
                            , [error localizedDescription]
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
                    , [error localizedDescription]
                );
                
                break;
            case FIRRemoteConfigFetchStatusThrottled:
                NSLog( @"FIRRemoteConfigFetchStatusThrottled: %@"
                    , [error localizedDescription]
                );
                
                break;
            default:
                break;
        }
    }];
    
    return YES;
}

@end
