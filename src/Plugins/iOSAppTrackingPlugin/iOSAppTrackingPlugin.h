#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "Environment/iOS/iOSPluginTransparencyConsentDelegateInterface.h"

#import "iOSAppTrackingInterface.h"

#define PLUGIN_BUNDLE_NAME "MengineiOSAdvertisementPlugin"

@interface iOSAppTrackingPlugin : NSObject<iOSPluginInterface, iOSAppTrackingInterface, iOSPluginTransparencyConsentDelegateInterface>

+ (instancetype)sharedInstance;

@property (nonatomic, assign) EiOSAppTrackingAuthorization m_status;
@property (nonatomic, strong) NSString * m_idfa;

@end
