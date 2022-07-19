#pragma once

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>
#import <DTBiOSSDK/DTBAdCallback.h>

@interface AppleAppLovinBannerDelegate : NSObject<MAAdViewAdDelegate, DTBAdCallback>

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) AdUnitIdentifier
                                amazonBannerSlotId:(NSString * _Nullable) amazonAdSlotId
                                              rect:(CGRect) rect;

- (void) show;
- (void) hide;

- (UIViewController* _Nullable) rootViewController;

@property (nonatomic, strong) MAAdView * _Nonnull m_adView;

@end
