#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import "AppleAppLovinBannerDelegate.h"
#import "AppleAppLovinInterstitialDelegate.h"
#import "AppleAppLovinRewardedDelegate.h"

#import <AppLovinSDK/AppLovinSDK.h>

#define PLUGIN_BUNDLE_NAME "MengineAppleAppLovinPlugin"

@interface AppleAppLovinApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface>

+ (AppleAppLovinApplicationDelegate *) sharedInstance;

- (void) setBannerProvider:(const Mengine::AppleAppLovinBannerProviderInterfacePtr &)bannerProvider;
- (Mengine::AppleAppLovinBannerProviderInterfacePtr) getBannerProvider;

- (void) setInterstitialProvider:(const Mengine::AppleAppLovinInterstitialProviderInterfacePtr &)interstitialProvider;
- (Mengine::AppleAppLovinInterstitialProviderInterfacePtr) getInterstitialProvider;

- (void) setRewardedProvider:(const Mengine::AppleAppLovinRewardedProviderInterfacePtr &)rewardedProvider;
- (Mengine::AppleAppLovinRewardedProviderInterfacePtr) getRewardedProvider;

- (AppleAppLovinBannerDelegate *) getBanner;
- (AppleAppLovinInterstitialDelegate *) getInterstitial;
- (AppleAppLovinRewardedDelegate *) getRewarded;

@property (nonatomic) Mengine::AppleAppLovinBannerProviderInterfacePtr m_bannerProvider;
@property (nonatomic) Mengine::AppleAppLovinInterstitialProviderInterfacePtr m_interstitialProvider;
@property (nonatomic) Mengine::AppleAppLovinRewardedProviderInterfacePtr m_rewardedProvider;

@property (nonatomic, strong) AppleAppLovinBannerDelegate * m_bannerAd;
@property (nonatomic, strong) AppleAppLovinInterstitialDelegate * m_interstitialAd;
@property (nonatomic, strong) AppleAppLovinRewardedDelegate * m_rewardedAd;

#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
@property (assign) AppleAppLovinAmazonService * m_amazonService;
#endif

@end
