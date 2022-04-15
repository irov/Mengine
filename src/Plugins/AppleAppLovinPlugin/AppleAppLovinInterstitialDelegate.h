#pragma once

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>

@interface AppleAppLovinInterstitialDelegate : NSObject<MAAdDelegate>

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString* _Nonnull) adUnitIdentifier;

- (BOOL) hasLoaded;
- (BOOL) show;

@property (nonatomic, strong) MAInterstitialAd * _Nonnull m_interstitialAd;
@property (nonatomic, assign) NSInteger m_retryAttempt;

@end
