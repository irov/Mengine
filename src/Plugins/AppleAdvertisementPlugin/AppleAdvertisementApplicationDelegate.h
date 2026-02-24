#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginConfigDelegateInterface.h"

#import "AppleAdvertisementInterface.h"
#import "AppleAdvertisementInterstitialPoint.h"
#import "AppleAdvertisementRewardedPoint.h"

#define PLUGIN_BUNDLE_NAME "MengineAppleAdvertisementPlugin"

@interface AppleAdvertisementApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, AppleAdvertisementInterface, iOSPluginConfigDelegateInterface>

- (instancetype)init;
+ (instancetype) sharedInstance;

@property (nonatomic, strong) id<AppleAdvertisementProviderInterface> m_provider;

@property (nonatomic, strong) id<AppleAdvertisementCallbackInterface> m_bannerCallback;
@property (nonatomic, strong) id<AppleAdvertisementCallbackInterface> m_interstitialCallback;
@property (nonatomic, strong) id<AppleAdvertisementCallbackInterface> m_rewardedCallback;

@property (nonatomic) BOOL m_optionNoAds;
@property (nonatomic) BOOL m_noAds;

@property (nonatomic) NSInteger m_lastShowInterstitial;
@property (nonatomic) NSInteger m_lastShowRewarded;

@property (nonatomic) NSInteger m_countShowInterstitial;
@property (nonatomic) NSInteger m_countShowRewarded;

@property (nonatomic, strong) NSMutableDictionary<NSString *, AppleAdvertisementAttempts *> * m_attempts;
@property (nonatomic, strong) NSMutableDictionary<NSString *, AppleAdvertisementCooldown *> * m_cooldowns;

@property (nonatomic, strong) NSDictionary<NSString *, AppleAdvertisementInterstitialPoint *> * m_interstitialPoints;
@property (nonatomic, strong) NSDictionary<NSString *, AppleAdvertisementRewardedPoint *> * m_rewardedPoints;

@end
