#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

@interface AppleFirebasePlugin : NSObject<iOSPluginInterface>

+ (instancetype)sharedInstance;

@end
