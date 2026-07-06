#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "Environment/iOS/iOSPluginLoggerDelegateInterface.h"
#import "Environment/iOS/iOSPluginAnalyticDelegateInterface.h"
#import "Environment/iOS/iOSPluginAdRevenueDelegateInterface.h"

@interface iOSPilotPlugin : NSObject<iOSPluginInterface, iOSPluginLoggerDelegateInterface, iOSPluginAnalyticDelegateInterface, iOSPluginAdRevenueDelegateInterface>

@end
