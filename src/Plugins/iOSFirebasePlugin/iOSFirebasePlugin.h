#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

@interface iOSFirebasePlugin : NSObject<iOSPluginInterface>

+ (instancetype)sharedInstance;

@end
