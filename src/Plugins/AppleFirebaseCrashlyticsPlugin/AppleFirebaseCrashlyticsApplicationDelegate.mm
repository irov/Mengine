#import "AppleFirebaseCrashlyticsApplicationDelegate.h"

#import "Environment/Apple/AppleString.h"
#import "Environment/iOS/iOSApplication.h"

#import <FirebaseCrashlytics/FirebaseCrashlytics.h>

#define PLUGIN_BUNDLE_NAME "MengineAppleFirebaseAnalyticsPlugin"

@implementation AppleFirebaseCrashlyticsApplicationDelegate

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [[FIRCrashlytics crashlytics] setCrashlyticsCollectionEnabled:MENGINE_DEBUG_VALUE(NO, YES)];
    
    NSString * sessionId = [iOSApplication.sharedInstance getSessionId];
    
    [[FIRCrashlytics crashlytics] setUserID:sessionId];
    
    return YES;
}

- (void)onEvent:(AppleEvent *)event args:(NSArray *)args {
    if (event == AppleEvent.EVENT_SESSION_ID) {
        NSString * sessionId = args[0];
        
        [[FIRCrashlytics crashlytics] setUserID:sessionId];
    }
}

- (void)onLogger:(iOSLogRecordParam *)record {
    switch (record.LOG_LEVEL) {
        case Mengine::LM_ERROR: {
            if ((record.LOG_FILTER & Mengine::LFILTER_EXCEPTION) == Mengine::LFILTER_EXCEPTION) {
                NSDictionary *userInfo = @{
                    NSLocalizedDescriptionKey: record.LOG_DATA
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
                NSLocalizedDescriptionKey: record.LOG_DATA
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
