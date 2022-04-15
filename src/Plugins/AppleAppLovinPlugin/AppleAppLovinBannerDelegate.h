#pragma once

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>

@interface AppleAppLovinBannerDelegate : NSObject<MAAdViewAdDelegate>

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) AdUnitIdentifier rect:(CGRect) rect;

- (void) show;
- (void) hide;

- (UIViewController* _Nullable) rootViewController;

@property (nonatomic, strong) MAAdView * _Nonnull m_adView;

@end
