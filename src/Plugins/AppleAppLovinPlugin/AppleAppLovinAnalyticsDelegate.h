#pragma once

#import <AppLovinSDK/AppLovinSDK.h>

@protocol AppleAppLovinAnalyticsDelegate<NSObject>

- (void) eventRevenuePaid:(MAAd * _Nonnull) ad;

@end
