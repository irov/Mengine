#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import "AppleGeneralDataProtectionRegulationInterface.h"

@interface AppleGeneralDataProtectionRegulationApplicationDelegate : NSObject<AppleGeneralDataProtectionRegulationInterface, iOSPluginApplicationDelegateInterface>

@end
