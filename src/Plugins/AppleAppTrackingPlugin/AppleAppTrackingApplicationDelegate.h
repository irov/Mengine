#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginTransparencyConsentDelegateInterface.h"

#import "AppleAppTrackingInterface.h"

#define PLUGIN_BUNDLE_NAME "MengineAppleAdvertisementPlugin"

@interface AppleAppTrackingApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, AppleAppTrackingInterface, iOSPluginTransparencyConsentDelegateInterface>

@property (nonatomic, assign) EAppleAppTrackingAuthorization m_status;
@property (nonatomic, strong) NSString * m_idfa;

@end
