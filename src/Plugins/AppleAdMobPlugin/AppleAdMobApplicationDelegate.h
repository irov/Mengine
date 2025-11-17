#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import "Plugins/AppleAdvertisementPlugin/AppleAdvertisementInterface.h"

#import "AppleAdMobInterface.h"

#import "AppleAdMobBannerDelegate.h"
#import "AppleAdMobInterstitialDelegate.h"
#import "AppleAdMobRewardedDelegate.h"

#import <GoogleMobileAds/GoogleMobileAds.h>

#define PLUGIN_BUNDLE_NAME "MengineAppleAdMobPlugin"

@interface AppleAdMobApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, AppleAdMobInterface, AppleAdvertisementProviderInterface>

- (id<AppleAdvertisementCallbackInterface>)getAdvertisementBannerCallback;
- (id<AppleAdvertisementCallbackInterface>)getAdvertisementInterstitialCallback;
- (id<AppleAdvertisementCallbackInterface>)getAdvertisementRewardedCallback;

- (AppleAdMobBannerDelegate *)getBanner;
- (AppleAdMobInterstitialDelegate *)getInterstitial;
- (AppleAdMobRewardedDelegate *)getRewarded;

@property (nonatomic, strong) AppleAdMobBannerDelegate * m_bannerAd;
@property (nonatomic, strong) AppleAdMobInterstitialDelegate * m_interstitialAd;
@property (nonatomic, strong) AppleAdMobRewardedDelegate * m_rewardedAd;

@end

