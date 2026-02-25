#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

#import <FirebaseMessaging/FirebaseMessaging.h>

@interface AppleFirebaseMessagingPlugin : NSObject<iOSPluginInterface, FIRMessagingDelegate>

@end
