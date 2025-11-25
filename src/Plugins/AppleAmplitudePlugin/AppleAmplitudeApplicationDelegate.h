#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginUserIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginAdRevenueDelegateInterface.h"
#import "Environment/iOS/iOSPluginAnalyticDelegateInterface.h"

@interface AppleAmplitudeApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, iOSPluginUserIdDelegateInterface, iOSPluginAdRevenueDelegateInterface, iOSPluginAnalyticDelegateInterface>

@end

