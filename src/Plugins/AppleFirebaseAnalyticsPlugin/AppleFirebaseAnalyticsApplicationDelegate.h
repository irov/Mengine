#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginSessionIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginAdRevenueDelegateInterface.h"
#import "Environment/iOS/iOSPluginTransparencyConsentDelegateInterface.h"

@interface AppleFirebaseAnalyticsApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, iOSPluginAdRevenueDelegateInterface, iOSPluginTransparencyConsentDelegateInterface, iOSPluginSessionIdDelegateInterface>

+ (AppleFirebaseAnalyticsApplicationDelegate *) sharedInstance;

@end


