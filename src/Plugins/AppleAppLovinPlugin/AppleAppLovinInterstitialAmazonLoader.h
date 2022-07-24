#pragma once

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>
#import <DTBiOSSDK/DTBAdCallback.h>

@interface AppleAppLovinInterstitialAmazonLoader : NSObject<DTBAdCallback>

- (instancetype _Nonnull) initWithSlotId:(NSString* _Nullable)amazonAdSlotId interstitialAd:(MAInterstitialAd* _Nonnull) interstitialAd;

@property (nonatomic, strong) MAInterstitialAd * _Nonnull m_interstitialAd;

@end
