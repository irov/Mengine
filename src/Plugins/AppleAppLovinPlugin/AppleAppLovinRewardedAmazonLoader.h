#pragma once

#import <AppLovinSDK/AppLovinSDK.h>
#import <Foundation/Foundation.h>
#import <DTBiOSSDK/DTBAdCallback.h>

@interface AppleAppLovinRewardedAmazonLoader : NSObject<DTBAdCallback>

- (instancetype _Nonnull) initWithSlotId:(NSString * _Nonnull) amazonAdSlotId rewardedAd:(MARewardedAd *_Nonnull) rewardedAd;

@property (nonatomic, strong) MARewardedAd *_Nonnull m_rewardedAd;

@end
