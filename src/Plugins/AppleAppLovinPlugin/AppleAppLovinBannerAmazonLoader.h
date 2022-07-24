#pragma once

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>
#import <DTBiOSSDK/DTBAdCallback.h>

@interface AppleAppLovinBannerAmazonLoader : NSObject<DTBAdCallback>

- (instancetype _Nonnull) initWithSlotId:(NSString * _Nullable) amazonAdSlotId adView:(MAAdView * _Nonnull) adView rect:(CGRect) rect;

@property (nonatomic, strong) MAAdView * _Nonnull m_adView;

@end
