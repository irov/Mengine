#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

#import "AppleNativePythonInterface.h"

@interface AppleNativePythonPlugin : NSObject<iOSPluginInterface, AppleNativePythonInterface>

@end
