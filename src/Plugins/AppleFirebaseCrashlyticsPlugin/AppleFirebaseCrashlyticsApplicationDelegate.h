#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginLoggerDelegateInterface.h"

@interface AppleFirebaseCrashlyticsApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, iOSPluginLoggerDelegateInterface>

@end
