#import "Environment/iOS/UIKitProxyApplicationDelegateBase.h"

#import <FirebaseMessaging/FirebaseMessaging.h>

@interface AppleFirebaseMessagingApplicationDelegate : NSObject<UIKitProxyApplicationDelegateInterface, FIRMessagingDelegate>

@end
