#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

#import "iOSFirebaseMessagingInterface.h"

#import <FirebaseMessaging/FirebaseMessaging.h>

@interface iOSFirebaseMessagingPlugin : NSObject<iOSPluginInterface, FIRMessagingDelegate, iOSFirebaseMessagingInterface>

@property (nonatomic, strong, nullable) NSString * m_pushToken;

@end
