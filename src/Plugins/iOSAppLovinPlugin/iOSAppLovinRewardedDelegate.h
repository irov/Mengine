#pragma once

#import "iOSAppLovinBaseDelegate.h"

#import "Environment/Apple/AppleIncluder.h"

#import "Configuration/Configurations.h"

#if defined(MENGINE_PLUGIN_IOS_APPLOVIN_MEDIATION_AMAZON)
#import "iOSAppLovinRewardedAmazonLoader.h"
#endif

#import <AppLovinSDK/AppLovinSDK.h>

@interface iOSAppLovinRewardedDelegate : iOSAppLovinBaseDelegate<MAAdRequestDelegate, MARewardedAdDelegate, MAAdRevenueDelegate, MAAdExpirationDelegate, MAAdReviewDelegate>

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull)adUnitId advertisement:(id<iOSAdvertisementInterface> _Nonnull)advertisement;

- (BOOL) canOffer:(NSString * _Nonnull)placement;
- (BOOL) canYouShow:(NSString * _Nonnull)placement;
- (BOOL) show:(NSString * _Nonnull)placement;
- (BOOL) isShowing;

- (void) loadAd;

@property (nonatomic, strong) MARewardedAd * _Nullable m_rewardedAd;

@property (atomic, assign) BOOL m_showing;

#if defined(MENGINE_PLUGIN_IOS_APPLOVIN_MEDIATION_AMAZON)
@property (nonatomic, strong) iOSAppLovinRewardedAmazonLoader * _Nullable m_amazonLoader;
#endif

@end
