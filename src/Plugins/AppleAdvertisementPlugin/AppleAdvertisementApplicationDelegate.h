#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginConfigDelegateInterface.h"

#import "AppleAdvertisementInterface.h"

#define PLUGIN_BUNDLE_NAME "MengineAppleAdvertisementPlugin"

@interface AppleAdvertisementApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, AppleAdvertisementInterface, iOSPluginConfigDelegateInterface>

@property (nonatomic, strong) id<AppleAdvertisementProviderInterface> m_provider;

@property (nonatomic, strong) id<AppleAdvertisementCallbackInterface> m_bannerCallback;
@property (nonatomic, strong) id<AppleAdvertisementCallbackInterface> m_interstitialCallback;
@property (nonatomic, strong) id<AppleAdvertisementCallbackInterface> m_rewardedCallback;

@end
