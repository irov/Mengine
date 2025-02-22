#import "AppleFirebaseCrashlyticsApplicationDelegate.h"

#import "Environment/Apple/AppleString.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSApplication.h"

#import <FirebaseCrashlytics/FirebaseCrashlytics.h>

#define PLUGIN_BUNDLE_NAME "MengineAppleFirebaseAnalyticsPlugin"

@implementation AppleFirebaseCrashlyticsApplicationDelegate

+ (AppleFirebaseCrashlyticsApplicationDelegate *) sharedInstance {
    static AppleFirebaseCrashlyticsApplicationDelegate *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleFirebaseCrashlyticsApplicationDelegate class]];
    });
    return sharedInstance;
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [[FIRCrashlytics crashlytics] setCrashlyticsCollectionEnabled:MENGINE_DEBUG_VALUE(NO, YES)];
    
    NSString * sessionId = [iOSApplication.sharedInstance getSessionId];
    
    [[FIRCrashlytics crashlytics] setUserID:sessionId];
    
    self.m_ANRMonitor = [[AppleFirebaseCrashlyticsANRMonitor alloc] init];
    
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
