#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "Environment/iOS/iOSPluginConfigDelegateInterface.h"

#import "iOSAdvertisementInterface.h"
#import "iOSAdvertisementInterstitialPoint.h"
#import "iOSAdvertisementRewardedPoint.h"

#define PLUGIN_BUNDLE_NAME "MengineiOSAdvertisementPlugin"

@interface iOSAdvertisementPlugin : NSObject<iOSPluginInterface, iOSAdvertisementInterface, iOSPluginConfigDelegateInterface>

- (instancetype)init;
+ (instancetype)sharedInstance;

@property (nonatomic, strong) id<iOSAdvertisementProviderInterface> m_provider;

@property (nonatomic, strong) id<iOSAdvertisementCallbackInterface> m_bannerCallback;
@property (nonatomic, strong) id<iOSAdvertisementCallbackInterface> m_interstitialCallback;
@property (nonatomic, strong) id<iOSAdvertisementCallbackInterface> m_rewardedCallback;

@property (nonatomic) BOOL m_optionNoAds;
@property (nonatomic) BOOL m_noAds;

@property (nonatomic) NSInteger m_lastShowInterstitial;
@property (nonatomic) NSInteger m_lastShowRewarded;

@property (nonatomic) NSInteger m_countShowInterstitial;
@property (nonatomic) NSInteger m_countShowRewarded;

@property (nonatomic, strong) NSMutableDictionary<NSString *, iOSAdvertisementAttempts *> * m_attempts;
@property (nonatomic, strong) NSMutableDictionary<NSString *, iOSAdvertisementCooldown *> * m_cooldowns;

@property (nonatomic, strong) NSDictionary<NSString *, iOSAdvertisementInterstitialPoint *> * m_interstitialPoints;
@property (nonatomic, strong) NSDictionary<NSString *, iOSAdvertisementRewardedPoint *> * m_rewardedPoints;

@end
