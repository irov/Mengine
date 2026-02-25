#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "Environment/iOS/iOSPluginTransparencyConsentDelegateInterface.h"

#import "AppleAppTrackingInterface.h"

#define PLUGIN_BUNDLE_NAME "MengineAppleAdvertisementPlugin"

@interface AppleAppTrackingPlugin : NSObject<iOSPluginInterface, AppleAppTrackingInterface, iOSPluginTransparencyConsentDelegateInterface>

+ (instancetype) sharedInstance;

@property (nonatomic, assign) EAppleAppTrackingAuthorization m_status;
@property (nonatomic, strong) NSString * m_idfa;

@end
