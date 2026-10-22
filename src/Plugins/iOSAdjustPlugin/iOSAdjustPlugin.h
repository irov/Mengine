#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

#import <AdjustSdk/Adjust.h>
#import <AdjustSdk/ADJConfig.h>

@interface iOSAdjustPlugin : NSObject<iOSPluginInterface, AdjustDelegate>

+ (instancetype)sharedInstance;

- (void)eventTraking:(NSString *)token;
- (void)revenueTracking:(NSString *)token amount:(double)amount currency:(NSString *)currency;

@end
