#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginUserIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginLoggerDelegateInterface.h"

#import "AppleFirebaseCrashlyticsInterface.h"

@interface AppleFirebaseCrashlyticsApplicationDelegate : NSObject<AppleFirebaseCrashlyticsInterface, iOSPluginApplicationDelegateInterface, iOSPluginLoggerDelegateInterface, iOSPluginUserIdDelegateInterface>

@end
