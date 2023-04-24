#pragma once

#include "AppleAppLovinInterface.h"

#import <AppLovinSDK/AppLovinSDK.h>
#import <Foundation/Foundation.h>

@interface AppleAppLovinBaseDelegate : NSObject

- (NSString *) getMAAdParams:(MAAd *) ad;

@property (nonatomic, assign) AppleAppLovinAnalyticsService * _Nonnull m_analytics;

@end
