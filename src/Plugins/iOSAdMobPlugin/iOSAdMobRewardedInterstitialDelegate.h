#pragma once

#import "iOSAdMobBaseDelegate.h"

@interface iOSAdMobRewardedInterstitialDelegate : iOSAdMobBaseDelegate<GADFullScreenContentDelegate>

- (BOOL)canYouShow:(NSString * _Nonnull)placement;
- (BOOL)show:(NSString * _Nonnull)placement;
- (BOOL)isShowing;

@property (nonatomic, strong) GADRewardedInterstitialAd * _Nullable m_ad;
@property (nonatomic, copy) NSString * _Nullable m_placement;
@property (nonatomic, assign) BOOL m_loading;
@property (nonatomic, assign) BOOL m_showing;
@property (nonatomic, assign) BOOL m_rewarded;
@property (nonatomic, assign) NSTimeInterval m_loadedTimestamp;

@end
