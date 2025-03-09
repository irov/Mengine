#import "AppleFirebaseRemoteConfigApplicationDelegate.h"

#import "Environment/iOS/iOSNetwork.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#include "AppleFirebaseRemoteConfigInterface.h"

#include "Kernel/Logger.h"

#import <FirebaseRemoteConfig/FirebaseRemoteConfig.h>

@implementation AppleFirebaseRemoteConfigApplicationDelegate

+ (instancetype) sharedInstance {
    static AppleFirebaseRemoteConfigApplicationDelegate *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleFirebaseRemoteConfigApplicationDelegate class]];
    });
    return sharedInstance;
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    self.m_configs = [NSMutableDictionary dictionary];
    
    FIRRemoteConfigSettings *remoteConfigSettings = [[FIRRemoteConfigSettings alloc] init];
    remoteConfigSettings.minimumFetchInterval = MENGINE_DEBUG_VALUE(0, 3600);
    
    FIRRemoteConfig * remoteConfig = [FIRRemoteConfig remoteConfig];
    
    [remoteConfig setConfigSettings:remoteConfigSettings];
    
    [remoteConfig setDefaultsFromPlistFileName:@MENGINE_FIREBASE_REMOTECONFIG_PLIST_NAME];
    
    return YES;
}

- (BOOL)application:(UIApplication *)application didPostLaunchingWithOptions:(NSDictionary *)launchOptions {
    FIRRemoteConfig * remoteConfig = [FIRRemoteConfig remoteConfig];
    
    [self updateRemoteConfigValues:remoteConfig];
    [self fetchRemoteConfigValues:remoteConfig];
    
    return YES;
}

- (void)fetchRemoteConfigValues:(FIRRemoteConfig *)remoteConfig {
    if ([[iOSNetwork sharedInstance] isNetworkAvailable] == NO) {
        return;
    }
     
    [remoteConfig fetchWithCompletionHandler:^(FIRRemoteConfigFetchStatus status, NSError *error) {
        switch (status) {
            case FIRRemoteConfigFetchStatusNoFetchYet: {
                IOS_LOGGER_MESSAGE(@"FIRRemoteConfigFetchStatusNoFetchYet: %@"
                                   , [error description]
                                   );
                
            } break;
            case FIRRemoteConfigFetchStatusSuccess: {
                IOS_LOGGER_MESSAGE(@"FIRRemoteConfigFetchStatusSuccess");
                
                [[FIRRemoteConfig remoteConfig] activateWithCompletion:^(BOOL changed, NSError * _Nullable error) {
                    if (error != nil) {
                        IOS_LOGGER_ERROR(@"FIRRemoteConfigFetchStatusSuccess activate error: %@"
                              , [error description]
                              );
                        
                        return;
                    }
                    
                    IOS_LOGGER_MESSAGE(@"FIRRemoteConfigFetchStatusSuccess activate changed: %d"
                          , changed
                          );
                    
                    if (changed == YES) {
                        [self updateRemoteConfigValues:remoteConfig];
                    }
                }];
            } break;
            case FIRRemoteConfigFetchStatusFailure: {
                IOS_LOGGER_ERROR(@"FIRRemoteConfigFetchStatusFailure: %@"
                                   , [error description]
                                   );
                
            } break;
            case FIRRemoteConfigFetchStatusThrottled: {
                IOS_LOGGER_MESSAGE(@"FIRRemoteConfigFetchStatusThrottled: %@"
                                   , [error description]
                                   );
                
            } break;
            default:
                break;
        }
    }];
}

- (void)updateRemoteConfigValues:(FIRRemoteConfig *)remoteConfig {
    NSArray<NSString *> * remoteKeys = [remoteConfig allKeysFromSource:FIRRemoteConfigSourceDefault];
    
    NSMutableDictionary * configs = [NSMutableDictionary dictionary];
    
    for( NSString * key in remoteKeys ) {
        FIRRemoteConfigValue * value = [remoteConfig configValueForKey:key];
        
        NSDictionary * json = [value JSONValue];
        
        if (json == nil) {
            IOS_LOGGER_MESSAGE(@"🔴 [ERROR] Apple firebase remote config not JSON: %@", key);
            
            continue;
        }
        
        [configs setObject:json forKey:key];
    }
    
    @synchronized (self) {
        self.m_configs = configs;
    }
    
    [iOSDetail config:configs];
}

- (BOOL)existRemoteConfigValue:(NSString *)key {
    @synchronized (self) {
        BOOL exist = [self.m_configs objectForKey:key] != nil;
        
        return exist;
    }
}    

- (NSDictionary *)getRemoteConfigValue:(NSString *)key {
    @synchronized (self) {
        NSDictionary * value = [self.m_configs objectForKey:key];
        
        return value;
    }
}

    

@end
