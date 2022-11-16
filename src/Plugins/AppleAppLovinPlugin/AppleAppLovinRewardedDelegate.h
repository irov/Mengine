#pragma once

#include "AppleAppLovinInterface.h"

#import <AppLovinSDK/AppLovinSDK.h>
#import <Foundation/Foundation.h>

#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
#import "AppleAppLovinRewardedAmazonLoader.h"
#endif

@interface AppleAppLovinRewardedDelegate : NSObject<MARewardedAdDelegate, MAAdRevenueDelegate>

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitIdentifier
                                      amazonSlotId:(NSString * _Nullable) amazonSlotId
                                    rewardCallback:(Mengine::AppleAppLovinRewardCallbackInterface * _Nonnull) callback;

- (BOOL) hasLoaded;
- (BOOL) show;

@property (assign) Mengine::AppleAppLovinRewardCallbackInterface * _Nonnull m_callback;
@property (nonatomic, strong) MARewardedAd * _Nullable m_rewardedAd;
@property (nonatomic, assign) NSInteger m_retryAttempt;

#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
@property (nonatomic, assign) AppleAppLovinRewardedAmazonLoader * _Nullable m_amazonLoader;
#endif

@end
