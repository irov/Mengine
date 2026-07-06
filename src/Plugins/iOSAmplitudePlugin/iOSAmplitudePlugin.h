#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "Environment/iOS/iOSPluginUserIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginAdRevenueDelegateInterface.h"
#import "Environment/iOS/iOSPluginAnalyticDelegateInterface.h"

@interface iOSAmplitudePlugin : NSObject<iOSPluginInterface, iOSPluginUserIdDelegateInterface, iOSPluginAdRevenueDelegateInterface, iOSPluginAnalyticDelegateInterface>

@end
