#pragma once

#import "AppleAppLovinAnalyticsService.h"
#import "AppleAppLovinInterface.h"

#import <AppLovinSDK/AppLovinSDK.h>
#import <Foundation/Foundation.h>

@interface AppleAppLovinBaseDelegate : NSObject

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                         analytics:(AppleAppLovinAnalyticsService * _Nonnull) analytics;

- (NSString * _Nonnull) getMAAdParams:(MAAd * _Nonnull) ad;
- (NSString * _Nonnull) getMAErrorParams:(MAError * _Nonnull) error;
- (NSString * _Nonnull) getMARewardParams:(MAReward * _Nonnull) reward;

@property (strong) NSString * _Nonnull m_adUnitId;
@property (nonatomic, strong) AppleAppLovinAnalyticsService * _Nonnull m_analytics;

@end
