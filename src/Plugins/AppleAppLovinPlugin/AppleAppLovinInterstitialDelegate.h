#pragma once

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>

#import "AppleAppLovinInterstitialAmazonLoader.h"

@interface AppleAppLovinInterstitialDelegate : NSObject<MAAdDelegate>

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString* _Nonnull) adUnitIdentifier
                                      amazonSlotId:(NSString* _Nullable) amazonSlotId;

- (BOOL) hasLoaded;
- (BOOL) show;

@property (nonatomic, strong) MAInterstitialAd * _Nonnull m_interstitialAd;
@property (nonatomic, assign) NSInteger m_retryAttempt;

@property (nonatomic, assign) AppleAppLovinInterstitialAmazonLoader * m_amazonLoader;

@end
