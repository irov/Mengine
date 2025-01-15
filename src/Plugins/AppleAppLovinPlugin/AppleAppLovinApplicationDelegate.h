#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import "AppleAppLovinBannerDelegate.h"
#import "AppleAppLovinInterstitialDelegate.h"
#import "AppleAppLovinRewardedDelegate.h"

#import <AppLovinSDK/AppLovinSDK.h>

#define PLUGIN_BUNDLE_NAME "MengineAppleAppLovinPlugin"

@interface AppleAppLovinApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface>

+ (AppleAppLovinApplicationDelegate *) sharedInstance;

- (void) setProvider:(const Mengine::AppleAppLovinProviderInterfacePtr &)provider;
- (Mengine::AppleAppLovinProviderInterfacePtr) getProvider;

- (AppleAppLovinBannerDelegate *) getBanner;
- (AppleAppLovinInterstitialDelegate *) getInterstitial;
- (AppleAppLovinRewardedDelegate *) getRewarded;

@property (nonatomic) Mengine::AppleAppLovinProviderInterfacePtr m_provider;

@property (nonatomic, strong) AppleAppLovinBannerDelegate * m_bannerAd;
@property (nonatomic, strong) AppleAppLovinInterstitialDelegate * m_interstitialAd;
@property (nonatomic, strong) AppleAppLovinRewardedDelegate * m_rewardedAd;

#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
@property (assign) AppleAppLovinAmazonService * m_amazonService;
#endif

@end
