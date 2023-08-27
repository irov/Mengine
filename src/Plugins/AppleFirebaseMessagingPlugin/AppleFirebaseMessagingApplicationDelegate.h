#import "Environment/iOS/UIProxyApplicationDelegateInterface.h"

#import <FirebaseMessaging/FirebaseMessaging.h>

@interface AppleFirebaseMessagingApplicationDelegate : NSObject<UIProxyApplicationDelegateInterface, FIRMessagingDelegate>

@end
