#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

@interface AppleFirebaseApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface>

+ (AppleFirebaseApplicationDelegate *) sharedInstance;

@end
