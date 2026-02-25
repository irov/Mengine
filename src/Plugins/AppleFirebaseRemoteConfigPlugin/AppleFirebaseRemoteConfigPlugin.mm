#import "AppleFirebaseRemoteConfigPlugin.h"

#import "Environment/iOS/iOSNetwork.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "AppleFirebaseRemoteConfigScriptEmbedding.h"
#endif

#include "Kernel/Logger.h"

#import <FirebaseRemoteConfig/FirebaseRemoteConfig.h>

@implementation AppleFirebaseRemoteConfigPlugin

- (instancetype)init {
    self = [super init];
    
    if (self) {
        self.m_configs = nil;
        self.m_ids = nil;
    }
    
    return self;
}

+ (instancetype)sharedInstance {
    static AppleFirebaseRemoteConfigPlugin *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleFirebaseRemoteConfigPlugin class]];
    });
    return sharedInstance;
}

#pragma mark - iOSPluginInterface

- (void)onRunBegin {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::addScriptEmbedding<Mengine::AppleFirebaseRemoteConfigScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE );
#endif
}

- (void)onStopEnd {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::removeScriptEmbedding<Mengine::AppleFirebaseRemoteConfigScriptEmbedding>();
#endif
}

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
    NSMutableDictionary * ids = [NSMutableDictionary dictionary];
    
    for( NSString * key in remoteKeys ) {
        FIRRemoteConfigValue * value = [remoteConfig configValueForKey:key];
        
        if (value == nil) {
            IOS_LOGGER_MESSAGE(@"Apple firebase remote config not value: %@", key);
            
            continue;
        }
        
        NSDictionary * json = [value JSONValue];
        
        if (json == nil) {
            IOS_LOGGER_ERROR(@"[ERROR] Apple firebase remote config not JSON: %@", key);
            
            continue;
        }
        
        [configs setObject:json forKey:key];
        
        id idValueObj = [json objectForKey:@"id"];
        
        if (idValueObj != nil && [idValueObj isKindOfClass:[NSNumber class]] == YES) {
            [ids setObject:(NSNumber *)idValueObj forKey:key];
        }
    }
    
    @synchronized (self) {
        self.m_configs = configs;
        self.m_ids = ids;
    }
    
    [iOSDetail config:configs ids:ids];
}

- (BOOL)hasRemoteConfig:(NSString *)key {
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
