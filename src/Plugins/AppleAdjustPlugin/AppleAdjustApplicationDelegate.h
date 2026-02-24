#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import <Adjust/Adjust.h>

@interface AppleAdjustApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, AdjustDelegate>

+ (instancetype)sharedInstance;

- (void)eventTraking:(NSString *)token;
- (void)revenueTracking:(NSString *)token amount:(double)amount currency:(NSString *)currency;

@end
