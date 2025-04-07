#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@protocol AppleGeneralDataProtectionRegulationInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)setGDPRPass:(BOOL) passGDPR;
- (BOOL)isGDPRPass;

@end
