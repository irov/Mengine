#import "AppleFirebaseRemoteConfigApplicationDelegate.h"

#include "AppleFirebaseRemoteConfigInterface.h"

#include "Kernel/Logger.h"

#import <FirebaseRemoteConfig/FirebaseRemoteConfig.h>

@implementation AppleFirebaseRemoteConfigApplicationDelegate

#pragma mark - UIPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    FIRRemoteConfigSettings *remoteConfigSettings = [[FIRRemoteConfigSettings alloc] init];
    remoteConfigSettings.minimumFetchInterval = 3600;
    [[FIRRemoteConfig remoteConfig] setConfigSettings:remoteConfigSettings];
    
    [[FIRRemoteConfig remoteConfig] setDefaultsFromPlistFileName:@"RemoteConfigDefaults"];
    
    [[FIRRemoteConfig remoteConfig] fetchWithCompletionHandler:^(FIRRemoteConfigFetchStatus status, NSError *error) {
        switch (status) {
            case FIRRemoteConfigFetchStatusNoFetchYet:
                LOGGER_ERROR("FIRRemoteConfigFetchStatusNoFetchYet: %s"
                    , [[error localizedDescription] UTF8String]
                );
                break;
            case FIRRemoteConfigFetchStatusSuccess:
                LOGGER_MESSAGE("FIRRemoteConfigFetchStatusSuccess");
                
                [[FIRRemoteConfig remoteConfig] activateWithCompletion:^(BOOL changed, NSError * _Nullable error) {
                    if (error != nil)
                    {
                        LOGGER_ERROR("FIRRemoteConfigFetchStatusSuccess activate error: %s"
                            , [[error localizedDescription] UTF8String]
                        );
                        
                        return;
                    }
                    
                    LOGGER_MESSAGE("FIRRemoteConfigFetchStatusSuccess activate changed: %d"
                        , changed
                    );
                }];
                break;
            case FIRRemoteConfigFetchStatusFailure:
                LOGGER_ERROR("FIRRemoteConfigFetchStatusFailure: %s"
                    , [[error localizedDescription] UTF8String]
                );
                
                break;
            case FIRRemoteConfigFetchStatusThrottled:
                LOGGER_ERROR("FIRRemoteConfigFetchStatusThrottled: %s"
                    , [[error localizedDescription] UTF8String]
                );
                
                break;
            default:
                break;
        }
    }];
}

@end
