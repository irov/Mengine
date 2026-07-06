#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "Environment/iOS/iOSPluginAppTrackingTransparencyDelegateInterface.h"

#import "Plugins/iOSAdvertisementPlugin/iOSAdvertisementInterface.h"

#import "iOSAdMobInterface.h"

#import "iOSAdMobBannerDelegate.h"
#import "iOSAdMobInterstitialDelegate.h"
#import "iOSAdMobRewardedDelegate.h"

#import <GoogleMobileAds/GoogleMobileAds.h>

#define PLUGIN_BUNDLE_NAME "MengineiOSAdMobPlugin"

@interface iOSAdMobPlugin : NSObject<iOSPluginInterface, iOSAdMobInterface, iOSAdvertisementProviderInterface, iOSPluginAppTrackingTransparencyDelegateInterface>

- (id<iOSAdvertisementCallbackInterface>)getAdvertisementBannerCallback;
- (id<iOSAdvertisementCallbackInterface>)getAdvertisementInterstitialCallback;
- (id<iOSAdvertisementCallbackInterface>)getAdvertisementRewardedCallback;

- (iOSAdMobBannerDelegate *)getBanner;
- (iOSAdMobInterstitialDelegate *)getInterstitial;
- (iOSAdMobRewardedDelegate *)getRewarded;

@property (nonatomic, strong) iOSAdMobBannerDelegate * m_bannerAd;
@property (nonatomic, strong) iOSAdMobInterstitialDelegate * m_interstitialAd;
@property (nonatomic, strong) iOSAdMobRewardedDelegate * m_rewardedAd;
@property (nonatomic, assign) BOOL m_initialized;

@end
