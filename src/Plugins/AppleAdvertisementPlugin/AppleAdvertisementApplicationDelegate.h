#pragma once

#import "Interface/AppleAdvertisementInterface.h"

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#define PLUGIN_BUNDLE_NAME "MengineAppleAdvertisementPlugin"

@interface AppleAdvertisementApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, AppleAdvertisementInterface>

@property (nonatomic, strong) id<AppleAdvertisementProviderInterface> m_provider;
@property (nonatomic, strong) id<AppleAdvertisementResponseInterface> m_response;

@end
