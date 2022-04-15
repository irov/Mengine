#pragma once

#include "AppleAppLovinInterface.h"

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>

@interface AppleAppLovinRewardedDelegate : NSObject<MARewardedAdDelegate>

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitIdentifier callback:(Mengine::AppLovinRewardCallbackInterface * _Nonnull) callback;

- (BOOL) hasLoaded;
- (BOOL) show;

@property (assign) Mengine::AppLovinRewardCallbackInterface * _Nonnull m_callback;
@property (nonatomic, strong) MARewardedAd *_Nonnull m_rewardedAd;
@property (nonatomic, assign) NSInteger m_retryAttempt;

@end
