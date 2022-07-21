#pragma once

#include "AppleAppLovinInterface.h"

#import <AppLovinSDK/AppLovinSDK.h>
#import <Foundation/Foundation.h>
#import <DTBiOSSDK/DTBAdCallback.h>

#import "AppleAppLovinRewardedAmazonLoader.h"

@interface AppleAppLovinRewardedDelegate : NSObject<MARewardedAdDelegate, DTBAdCallback>

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitIdentifier
                                      amazonSlotId:(NSString* _Nullable) amazonSlotId
                                    rewardCallback:(Mengine::AppleAppLovinRewardCallbackInterface * _Nonnull) callback;

- (BOOL) hasLoaded;
- (BOOL) show;

@property (assign) Mengine::AppleAppLovinRewardCallbackInterface * _Nonnull m_callback;
@property (nonatomic, strong) MARewardedAd *_Nonnull m_rewardedAd;
@property (nonatomic, assign) NSInteger m_retryAttempt;

@property (nonatomic, assign) AppleAppLovinRewardedAmazonLoader * m_amazonLoader;

@end
