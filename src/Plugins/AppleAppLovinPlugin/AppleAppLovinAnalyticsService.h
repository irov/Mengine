#pragma once

#import "AppleAppLovinAnalyticsDelegate.h"

#import <AppLovinSDK/AppLovinSDK.h>

@interface AppleAppLovinAnalyticsService : NSObject

- (instancetype _Nonnull) init;

- (void) addAnalyticsDelegate:(id<AppleAppLovinAnalyticsDelegate> _Nonnull) provider;

- (void) eventRevenuePaid:(MAAd * _Nonnull) ad;

@property (nonatomic, strong) NSMutableArray * _Nullable m_providers;

@end
