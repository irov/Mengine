#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

@interface AppleGeneralDataProtectionRegulationApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface>

- (void) setGDPRPass:(BOOL) passGDPR;
- (BOOL) isGDPRPass;

@end
