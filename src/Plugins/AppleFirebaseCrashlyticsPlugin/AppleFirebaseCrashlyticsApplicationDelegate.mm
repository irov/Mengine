#import "AppleFirebaseCrashlyticsApplicationDelegate.h"

#import "Environment/Apple/AppleString.h"
#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSLog.h"

#import <FirebaseCrashlytics/FirebaseCrashlytics.h>

#define PLUGIN_BUNDLE_NAME "MengineAppleFirebaseAnalyticsPlugin"

@implementation AppleFirebaseCrashlyticsApplicationDelegate

+ (instancetype) sharedInstance {
    static AppleFirebaseCrashlyticsApplicationDelegate *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleFirebaseCrashlyticsApplicationDelegate class]];
    });
    return sharedInstance;
}

- (void)log:(NSString *)message {
    [[FIRCrashlytics crashlytics] log:message];
}

- (void)setCustomValue:(NSString *)value forKey:(NSString *)key {
    [[FIRCrashlytics crashlytics] setCustomValue:value forKey:key];
}

- (void)setCustomKeysAndValues:(NSDictionary *)keysAndValues {
    [[FIRCrashlytics crashlytics] setCustomKeysAndValues:keysAndValues];
}
- (void)recordError:(NSString *)name errorCode:(NSInteger)code {
    IOS_LOGGER_MESSAGE( @"record error name: %@ code: %lu"
        , name
        , code
    );

    NSDictionary *userInfo = @{
      NSLocalizedDescriptionKey:name
    };
    
    NSError * error = [NSError errorWithDomain:@("com.mengine.firebase")
                                         code:code
                                     userInfo:userInfo];

    [[FIRCrashlytics crashlytics] recordError:error];
                                          
}

- (void)recordError:(NSError *)error {
    IOS_LOGGER_MESSAGE( @"record error: %@"
        , [AppleDetail getMessageFromNSError:error]
    );
    
    [[FIRCrashlytics crashlytics] recordError:error];
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [[FIRCrashlytics crashlytics] setCrashlyticsCollectionEnabled:MENGINE_DEBUG_VALUE(NO, YES)];
    
    NSString * sessionId = [iOSApplication.sharedInstance getSessionId];
    
    [[FIRCrashlytics crashlytics] setUserID:sessionId];
    
    return YES;
}

- (void)onSessionId:(iOSSessionIdParam *)session {
    [[FIRCrashlytics crashlytics] setUserID:session.SESSION_ID];
}

- (void)onRemoveSessionData {
    [[FIRCrashlytics crashlytics] deleteUnsentReports];
    [[FIRCrashlytics crashlytics] setUserID:nil];
}

- (void)onLogger:(AppleLogRecordParam *)record {
    switch (record.LOG_LEVEL) {
        case Mengine::LM_ERROR: {
            if ((record.LOG_FILTER & Mengine::LFILTER_EXCEPTION) == Mengine::LFILTER_EXCEPTION) {
                NSDictionary *userInfo = @{
                    NSLocalizedDescriptionKey:record.LOG_DATA
                };
            
                NSError * error = [NSError errorWithDomain:record.LOG_CATEGORY
                                                      code:0
                                                  userInfo:userInfo];
            
                [[FIRCrashlytics crashlytics] recordError:error];
            } else {
                [[FIRCrashlytics crashlytics] log:record.LOG_DATA];
            }
        }break;
        case Mengine::LM_FATAL: {
            NSDictionary *userInfo = @{
                NSLocalizedDescriptionKey:record.LOG_DATA
            };
            
            NSError * error = [NSError errorWithDomain:record.LOG_CATEGORY
                                                  code:0
                                              userInfo:userInfo];
            
            [[FIRCrashlytics crashlytics] recordError:error];
        }break;
        default:
            break;
    }
}

@end
