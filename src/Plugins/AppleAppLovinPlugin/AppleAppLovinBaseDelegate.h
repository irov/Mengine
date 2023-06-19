#pragma once

#import "AppleAppLovinAnalyticsService.h"

#import <AppLovinSDK/AppLovinSDK.h>
#import <Foundation/Foundation.h>

@interface AppleAppLovinBaseDelegate : NSObject

- (NSString * _Nonnull) getMAAdParams:(MAAd * _Nonnull) ad;
- (NSString * _Nonnull) getMAErrorParams:(MAError * _Nonnull) error;
- (NSString * _Nonnull) getMARewardParams:(MAReward * _Nonnull) reward;

@property (nonatomic, strong) AppleAppLovinAnalyticsService * _Nonnull m_analytics;

@end
