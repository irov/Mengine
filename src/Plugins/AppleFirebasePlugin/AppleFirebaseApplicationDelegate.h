#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

@interface AppleFirebaseApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface>

+ (instancetype)sharedInstance;

@end
