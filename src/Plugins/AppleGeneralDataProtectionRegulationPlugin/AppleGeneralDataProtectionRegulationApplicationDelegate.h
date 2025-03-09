#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

@interface AppleGeneralDataProtectionRegulationApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface>

+ (instancetype)sharedInstance;

- (void)setGDPRPass:(BOOL) passGDPR;
- (BOOL)isGDPRPass;

@end
