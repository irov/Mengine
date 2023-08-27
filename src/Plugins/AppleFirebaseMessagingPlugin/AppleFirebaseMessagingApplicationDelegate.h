#import "Environment/iOS/UIPluginApplicationDelegateInterface.h"

#import <FirebaseMessaging/FirebaseMessaging.h>

@interface AppleFirebaseMessagingApplicationDelegate : NSObject<UIPluginApplicationDelegateInterface, FIRMessagingDelegate>

@end
