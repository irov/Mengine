#pragma once

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>
#import <DTBiOSSDK/DTBAdCallback.h>

@interface AppleAppLovinInterstitialDelegate : NSObject<MAAdDelegate,DTBAdCallback>

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString* _Nonnull) adUnitIdentifier
                                 amazonInterSlotId:(NSString* _Nullable)amazonInterSlotId;

- (BOOL) hasLoaded;
- (BOOL) show;

@property (nonatomic, strong) MAInterstitialAd * _Nonnull m_interstitialAd;
@property (nonatomic, assign) NSInteger m_retryAttempt;

@end
