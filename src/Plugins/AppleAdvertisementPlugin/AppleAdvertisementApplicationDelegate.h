#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import "AppleAdvertisementInterface.h"

#define PLUGIN_BUNDLE_NAME "MengineAppleAdvertisementPlugin"

@interface AppleAdvertisementApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, AppleAdvertisementInterface>

@property (nonatomic, strong) id<AppleAdvertisementProviderInterface> m_provider;

@end
