#import "AppleFirebaseCrashlyticsApplicationDelegate.h"

#import "Environment/Apple/AppleString.h"
#import "Environment/iOS/iOSApplication.h"

#import <FirebaseCrashlytics/FirebaseCrashlytics.h>

#define PLUGIN_BUNDLE_NAME "MengineAppleFirebaseAnalyticsPlugin"

@implementation AppleFirebaseCrashlyticsApplicationDelegate

#pragma mark - UIPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
#ifdef MENGINE_DEBUG
    [[FIRCrashlytics crashlytics] setCrashlyticsCollectionEnabled:NO];
#else
    [[FIRCrashlytics crashlytics] setCrashlyticsCollectionEnabled:YES];
#endif
    
    NSString * sessionId = [iOSApplication.sharedInstance getSessionId];
    
    [[FIRCrashlytics crashlytics] setUserID:sessionId];
    
    return YES;
}

- (void)onEvent:(MengineEvent *)event args:(NSArray *)args {
    if (event == MengineEvent.EVENT_SESSION_ID) {
        NSString * sessionId = args[0];
        
        [[FIRCrashlytics crashlytics] setUserID:sessionId];
    }
}

- (void)onLog:(const Mengine::LoggerRecordInterfacePtr &)record {
    Mengine::LoggerMessage message;
    record->getMessage( &message );
    
    switch (message.level) {
        case Mengine::LM_ERROR: {
            NSString * ns_message = Mengine::Helper::stringToNSString(message.data, message.size);
            
            [[FIRCrashlytics crashlytics] log:ns_message];
        }break;
        case Mengine::LM_FATAL: {
            NSString * ns_message = Mengine::Helper::stringToNSString(message.data, message.size);
            
            NSDictionary *userInfo = @{
                NSLocalizedDescriptionKey: ns_message
            };
            
            NSError * error = [NSError errorWithDomain:@("com.mengine.firebase")
                                                  code:0
                                              userInfo:userInfo];
            
            [[FIRCrashlytics crashlytics] recordError:error];
        }break;
        default:
            break;
    }
}

@end
