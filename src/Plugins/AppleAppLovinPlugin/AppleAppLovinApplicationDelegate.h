#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import "Plugins/AppleAdvertisementPlugin/AppleAdvertisementInterface.h"

#import "AppleAppLovinInterface.h"

#import "AppleAppLovinBannerDelegate.h"
#import "AppleAppLovinInterstitialDelegate.h"
#import "AppleAppLovinRewardedDelegate.h"

#import <AppLovinSDK/AppLovinSDK.h>

#define PLUGIN_BUNDLE_NAME "MengineAppleAppLovinPlugin"

@interface AppleAppLovinApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, AppleAppLovinInterface, AppleAdvertisementProviderInterface>

- (id<AppleAdvertisementCallbackInterface>)getAdvertisementBannerCallback;
- (id<AppleAdvertisementCallbackInterface>)getAdvertisementInterstitialCallback;
- (id<AppleAdvertisementCallbackInterface>)getAdvertisementRewardedCallback;

- (AppleAppLovinBannerDelegate *)getBanner;
- (AppleAppLovinInterstitialDelegate *)getInterstitial;
- (AppleAppLovinRewardedDelegate *)getRewarded;

@property (nonatomic, strong) AppleAppLovinBannerDelegate * m_bannerAd;
@property (nonatomic, strong) AppleAppLovinInterstitialDelegate * m_interstitialAd;
@property (nonatomic, strong) AppleAppLovinRewardedDelegate * m_rewardedAd;

#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
@property (assign) AppleAppLovinAmazonService * m_amazonService;
#endif

@end
