#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

@interface AppleFirebaseRemoteConfigApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface>

+ (AppleFirebaseRemoteConfigApplicationDelegate *) sharedInstance;

@end
