#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import <AppLovinSDK/AppLovinSDK.h>

#define PLUGIN_BUNDLE_NAME "MengineAppleAppLovinPlugin"

@interface AppleAppLovinApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface>

@end
