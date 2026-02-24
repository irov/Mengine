#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import "AppleNativePythonInterface.h"

@interface AppleNativePythonApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, AppleNativePythonInterface>

@end
