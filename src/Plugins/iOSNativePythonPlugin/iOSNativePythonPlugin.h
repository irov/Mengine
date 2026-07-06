#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

#import "iOSNativePythonInterface.h"

@interface iOSNativePythonPlugin : NSObject<iOSPluginInterface, iOSNativePythonInterface>

@end
