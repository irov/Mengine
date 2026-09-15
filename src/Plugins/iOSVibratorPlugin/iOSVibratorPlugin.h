#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

#import "iOSVibratorInterface.h"

@interface iOSVibratorPlugin : NSObject<iOSPluginInterface, iOSVibratorInterface>

+ (instancetype)sharedInstance;

@end
