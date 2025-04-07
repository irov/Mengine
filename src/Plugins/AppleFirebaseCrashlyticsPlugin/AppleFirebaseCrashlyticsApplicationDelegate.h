#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginSessionIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginLoggerDelegateInterface.h"

#import "AppleFirebaseCrashlyticsInterface.h"

@interface AppleFirebaseCrashlyticsApplicationDelegate : NSObject<AppleFirebaseCrashlyticsInterface, iOSPluginApplicationDelegateInterface, iOSPluginLoggerDelegateInterface, iOSPluginSessionIdDelegateInterface>

@end
