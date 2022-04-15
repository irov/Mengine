#pragma once

#include "AppLovinInterface.h"

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>



@interface AppLovinRewardedDelegate : NSObject<MARewardedAdDelegate>

- (instancetype _Nonnull) initWithCallback:(Mengine::AppLovinRewardCallback * _Nonnull)callback WithAdUnitIdentifier:(NSString* _Nonnull) key;

- (BOOL) hasLoaded;
- (BOOL) show;


@property (assign) Mengine::AppLovinRewardCallback * _Nonnull m_callbacks;
@property (nonatomic, strong) MARewardedAd *_Nonnull rewardedAd;
@property (nonatomic, assign) NSInteger retryAttempt;
@end
