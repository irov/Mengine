#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

@interface AppleGeneralDataProtectionRegulationApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface>

+ (AppleGeneralDataProtectionRegulationApplicationDelegate *) sharedInstance;

- (void) setGDPRPass:(BOOL) passGDPR;
- (BOOL) isGDPRPass;

@end
