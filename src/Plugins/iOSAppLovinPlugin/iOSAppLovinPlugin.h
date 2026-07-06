#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

#import "Plugins/iOSAdvertisementPlugin/iOSAdvertisementInterface.h"

#import "iOSAppLovinInterface.h"

#import "iOSAppLovinBannerDelegate.h"
#import "iOSAppLovinInterstitialDelegate.h"
#import "iOSAppLovinRewardedDelegate.h"

#import <AppLovinSDK/AppLovinSDK.h>

#define PLUGIN_BUNDLE_NAME "MengineiOSAppLovinPlugin"

@interface iOSAppLovinPlugin : NSObject<iOSPluginInterface, iOSAppLovinInterface, iOSAdvertisementProviderInterface>

+ (instancetype)sharedInstance;

- (id<iOSAdvertisementCallbackInterface>)getAdvertisementBannerCallback;
- (id<iOSAdvertisementCallbackInterface>)getAdvertisementInterstitialCallback;
- (id<iOSAdvertisementCallbackInterface>)getAdvertisementRewardedCallback;

- (iOSAppLovinBannerDelegate *)getBanner;
- (iOSAppLovinInterstitialDelegate *)getInterstitial;
- (iOSAppLovinRewardedDelegate *)getRewarded;

@property (nonatomic, strong) iOSAppLovinBannerDelegate * m_bannerAd;
@property (nonatomic, strong) iOSAppLovinInterstitialDelegate * m_interstitialAd;
@property (nonatomic, strong) iOSAppLovinRewardedDelegate * m_rewardedAd;

#if defined(MENGINE_PLUGIN_IOS_APPLOVIN_MEDIATION_AMAZON)
@property (assign) iOSAppLovinAmazonService * m_amazonService;
#endif

@end
