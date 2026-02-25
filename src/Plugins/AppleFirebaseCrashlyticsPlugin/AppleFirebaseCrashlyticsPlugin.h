#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "Environment/iOS/iOSPluginUserIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginLoggerDelegateInterface.h"

#import "AppleFirebaseCrashlyticsInterface.h"

@interface AppleFirebaseCrashlyticsPlugin : NSObject<AppleFirebaseCrashlyticsInterface, iOSPluginInterface, iOSPluginLoggerDelegateInterface, iOSPluginUserIdDelegateInterface>

@end
