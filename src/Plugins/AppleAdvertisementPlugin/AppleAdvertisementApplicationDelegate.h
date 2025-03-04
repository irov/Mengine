#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#define PLUGIN_BUNDLE_NAME "MengineAppleAdvertisementPlugin"

@interface AppleAdvertisementApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface>

+ (AppleAdvertisementApplicationDelegate *) sharedInstance;

@end
