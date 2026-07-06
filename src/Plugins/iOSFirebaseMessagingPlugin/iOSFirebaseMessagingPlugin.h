#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

#import <FirebaseMessaging/FirebaseMessaging.h>

@interface iOSFirebaseMessagingPlugin : NSObject<iOSPluginInterface, FIRMessagingDelegate>

@end
