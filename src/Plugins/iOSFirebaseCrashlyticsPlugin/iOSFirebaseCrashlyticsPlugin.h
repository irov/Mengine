#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "Environment/iOS/iOSPluginUserIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginLoggerDelegateInterface.h"

#import "iOSFirebaseCrashlyticsInterface.h"

@interface iOSFirebaseCrashlyticsPlugin : NSObject<iOSFirebaseCrashlyticsInterface, iOSPluginInterface, iOSPluginLoggerDelegateInterface, iOSPluginUserIdDelegateInterface>

@end
