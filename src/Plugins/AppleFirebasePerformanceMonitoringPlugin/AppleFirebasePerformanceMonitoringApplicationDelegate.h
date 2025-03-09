#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import <FirebaseMessaging/FirebaseMessaging.h>

@interface AppleFirebaseMessagingApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, FIRMessagingDelegate>

@end
