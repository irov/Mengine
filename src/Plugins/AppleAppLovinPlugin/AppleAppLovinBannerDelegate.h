#pragma once

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>

#import "AppleAppLovinBannerAmazonLoader.h"

@interface AppleAppLovinBannerDelegate : NSObject<MAAdViewAdDelegate>

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString* _Nonnull) AdUnitIdentifier
                                      amazonSlotId:(NSString* _Nullable) amazonSlotId
                                              rect:(CGRect) rect;

- (void) show;
- (void) hide;

- (UIViewController* _Nullable) rootViewController;

@property (nonatomic, strong) MAAdView * _Nonnull m_adView;
@property (nonatomic, strong) AppleAppLovinBannerAmazonLoader * m_amazonLoader;

@end
